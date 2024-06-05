#pragma once

namespace mt {

    struct NonCopyable {
        NonCopyable& operator=(const NonCopyable&) = delete;
        NonCopyable(const NonCopyable&)            = delete;
        NonCopyable()                              = default;
    };

} // namespace mt
