#ifndef MOLTEN_CORE_GUI_DATA_OUTLINEDATA_HPP
#define MOLTEN_CORE_GUI_DATA_OUTLINEDATA_HPP

#include "Molten/Math/Vector.hpp"

namespace Molten::Gui
{

    /** Class used for representing padding and margin. */
    class MOLTEN_API OutlineData
    {

    public:

        /** Constructing outline data with all elements set to 0. */
        OutlineData();

        /** Constructing outline data with input parameters for each element. */
        OutlineData(
            const float left,
            const float top,
            const float right,
            const float bottom);

        MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
        union
        {
            struct
            {
                float left;
                float top;
            };
            Vector2f32 low;
        };
        union
        {
            struct
            {
                float right;
                float bottom;
            };
            Vector2f32 high;
        };
        MOLTEN_ANONYMOUS_STRUCTURE_END

    };

    using PaddingData = OutlineData; ///< Named padding type.
    using MarginData = OutlineData;  ///< Named margin type.

}

#endif