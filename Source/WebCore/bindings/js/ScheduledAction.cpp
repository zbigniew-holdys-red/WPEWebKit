/*
 *  Copyright (C) 2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006 Jon Shier (jshier@iastate.edu)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reseved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 *  Copyright (C) 2009 Google Inc. All rights reseved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#include "config.h"
#include "ScheduledAction.h"

#include "ContentSecurityPolicy.h"
#include "DOMWindow.h"
#include "Document.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "JSDOMBinding.h"
#include "JSDOMWindow.h"
#include "JSMainThreadExecState.h"
#include "JSMainThreadExecStateInstrumentation.h"
#include "JSWorkerGlobalScope.h"
#include "ScriptController.h"
#include "ScriptExecutionContext.h"
#include "ScriptSourceCode.h"
#include "WorkerGlobalScope.h"
#include "WorkerThread.h"
#include <runtime/JSLock.h>

using namespace JSC;

namespace WebCore {

std::unique_ptr<ScheduledAction> ScheduledAction::create(ExecState* exec, DOMWrapperWorld& isolatedWorld, ContentSecurityPolicy* policy)
{
    VM& vm = exec->vm();
    auto scope = DECLARE_THROW_SCOPE(vm);

    JSValue v = exec->argument(0);
    CallData callData;
    if (getCallData(v, callData) == CallType::None) {
        if (policy && !policy->allowEval(exec))
            return nullptr;
        String string = v.toString(exec)->value(exec);
        RETURN_IF_EXCEPTION(scope, nullptr);
        return std::unique_ptr<ScheduledAction>(new ScheduledAction(string, isolatedWorld));
    }

    return std::unique_ptr<ScheduledAction>(new ScheduledAction(exec, v, isolatedWorld));
}

ScheduledAction::ScheduledAction(ExecState* exec, JSValue function, DOMWrapperWorld& isolatedWorld)
    : m_function(exec->vm(), function)
    , m_isolatedWorld(&isolatedWorld)
{
    // setTimeout(function, interval, arg0, arg1...).
    // Start at 2 to skip function and interval.
    for (size_t i = 2; i < exec->argumentCount(); ++i)
        m_args.append(Strong<JSC::Unknown>(exec->vm(), exec->uncheckedArgument(i)));
}

void ScheduledAction::execute(ScriptExecutionContext& context)
{
    if (is<Document>(context))
        execute(downcast<Document>(context));
    else
        execute(downcast<WorkerGlobalScope>(context));
}

void ScheduledAction::executeFunctionInContext(JSGlobalObject* globalObject, JSValue thisValue, ScriptExecutionContext& context)
{
    ASSERT(m_function);
    JSLockHolder lock(context.vm());

    CallData callData;
    CallType callType = getCallData(m_function.get(), callData);
    if (callType == CallType::None)
        return;

    ExecState* exec = globalObject->globalExec();

    MarkedArgumentBuffer args;
    size_t size = m_args.size();
    for (size_t i = 0; i < size; ++i)
        args.append(m_args[i].get());

    InspectorInstrumentationCookie cookie = JSMainThreadExecState::instrumentFunctionCall(&context, callType, callData);

    NakedPtr<JSC::Exception> exception;
    if (is<Document>(context))
        JSMainThreadExecState::profiledCall(exec, JSC::ProfilingReason::Other, m_function.get(), callType, callData, thisValue, args, exception);
    else
        JSC::profiledCall(exec, JSC::ProfilingReason::Other, m_function.get(), callType, callData, thisValue, args, exception);

    InspectorInstrumentation::didCallFunction(cookie, &context);

    if (exception)
        reportException(exec, exception);
}

void ScheduledAction::execute(Document& document)
{
    JSDOMWindow* window = toJSDOMWindow(document.frame(), *m_isolatedWorld);
    if (!window)
        return;

    RefPtr<Frame> frame = window->wrapped().frame();
    if (!frame || !frame->script().canExecuteScripts(AboutToExecuteScript))
        return;

    if (m_function)
        executeFunctionInContext(window, window->shell(), document);
    else
        frame->script().executeScriptInWorld(*m_isolatedWorld, m_code);
}

void ScheduledAction::execute(WorkerGlobalScope& workerGlobalScope)
{
    // In a Worker, the execution should always happen on a worker thread.
    ASSERT(workerGlobalScope.thread().threadID() == currentThread());

    WorkerScriptController* scriptController = workerGlobalScope.script();

    if (m_function) {
        JSWorkerGlobalScope* contextWrapper = scriptController->workerGlobalScopeWrapper();
        executeFunctionInContext(contextWrapper, contextWrapper, workerGlobalScope);
    } else {
        ScriptSourceCode code(m_code, workerGlobalScope.url());
        scriptController->evaluate(code);
    }
}

} // namespace WebCore
