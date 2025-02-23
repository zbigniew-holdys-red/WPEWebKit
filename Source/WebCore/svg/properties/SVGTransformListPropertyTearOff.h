/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#pragma once

#include "SVGListPropertyTearOff.h"
#include "SVGTransformList.h"

namespace WebCore {

class SVGTransformListPropertyTearOff final : public SVGListPropertyTearOff<SVGTransformList> {
public:
    using AnimatedListPropertyTearOff = SVGAnimatedListPropertyTearOff<SVGTransformList>;
    using ListWrapperCache = AnimatedListPropertyTearOff::ListWrapperCache;

    static Ref<SVGListPropertyTearOff<SVGTransformList>> create(AnimatedListPropertyTearOff* animatedProperty, SVGPropertyRole role, SVGTransformList& values, ListWrapperCache& wrappers)
    {
        ASSERT(animatedProperty);
        return adoptRef(*new SVGTransformListPropertyTearOff(animatedProperty, role, values, wrappers));
    }

    ExceptionOr<Ref<SVGPropertyTearOff<SVGTransform>>> createSVGTransformFromMatrix(SVGPropertyTearOff<SVGMatrix>* matrix)
    {
        ASSERT(m_values);
        if (!matrix)
            return Exception { TYPE_MISMATCH_ERR };
        return SVGPropertyTearOff<SVGTransform>::create(m_values->createSVGTransformFromMatrix(matrix->propertyReference()));
    }

    ExceptionOr<RefPtr<SVGPropertyTearOff<SVGTransform>>> consolidate()
    {
        ASSERT(m_values);
        ASSERT(m_wrappers);

        auto result = canAlterList();
        if (result.hasException())
            return result.releaseException();
        if (!result.releaseReturnValue())
            return nullptr;

        ASSERT(m_values->size() == m_wrappers->size());

        // Spec: If the list was empty, then a value of null is returned.
        if (m_values->isEmpty())
            return nullptr;

        detachListWrappers(0);
        RefPtr<SVGPropertyTearOff<SVGTransform>> wrapper = SVGPropertyTearOff<SVGTransform>::create(m_values->consolidate());
        m_wrappers->append(wrapper);

        ASSERT(m_values->size() == m_wrappers->size());
        return WTFMove(wrapper);
    }

private:
    SVGTransformListPropertyTearOff(AnimatedListPropertyTearOff* animatedProperty, SVGPropertyRole role, SVGTransformList& values, ListWrapperCache& wrappers)
        : SVGListPropertyTearOff<SVGTransformList>(animatedProperty, role, values, wrappers)
    {
    }
};

}
