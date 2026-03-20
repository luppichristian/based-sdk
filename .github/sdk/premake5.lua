local based_sdk = {}

local function sdk_os_name()
    local host = os.target()
    if host == "windows" then
        return "windows"
    end
    if host == "linux" then
        return "linux"
    end
    if host == "macosx" then
        return "macos"
    end
    error("Unsupported platform '" .. tostring(host) .. "'")
end

local function variant_dir(sdk_root, os_name, arch_name, config_name)
    return path.join(sdk_root, "variants", os_name .. "-" .. arch_name .. "-" .. config_name)
end

local function add_variant_filters(sdk_root, os_name, arch_name)
    local release_variant = variant_dir(sdk_root, os_name, arch_name, "Release")
    local include_root = path.join(release_variant, "include")

    includedirs {
        path.join(include_root, "core"),
        path.join(include_root, "gfx"),
        path.join(include_root, "third_party", "olib"),
        path.join(include_root, "third_party", "libmath2"),
        path.join(include_root, "third_party", "tracy"),
    }

    links { "based-core", "based-gfx" }

    filter { "configurations:Debug" }
        libdirs { path.join(variant_dir(sdk_root, os_name, arch_name, "Debug"), "lib") }

    filter { "configurations:Release" }
        libdirs { path.join(variant_dir(sdk_root, os_name, arch_name, "Release"), "lib") }

    filter { "configurations:RelWithDebInfo" }
        libdirs { path.join(variant_dir(sdk_root, os_name, arch_name, "RelWithDebInfo"), "lib") }

    filter { "configurations:MinSizeRel" }
        libdirs { path.join(variant_dir(sdk_root, os_name, arch_name, "MinSizeRel"), "lib") }
end

function based_sdk.use(sdk_root)
    local os_name = sdk_os_name()

    filter { "architecture:x86_64" }
        add_variant_filters(sdk_root, os_name, "x86_64")

    filter { "architecture:ARM64" }
        add_variant_filters(sdk_root, os_name, "arm64")

    filter { "architecture:arm64" }
        add_variant_filters(sdk_root, os_name, "arm64")

    filter {}
end

return based_sdk
