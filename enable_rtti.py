Import("env")
import re

def enable_rtti_for_file(node):
    # Convert node path to use forward slashes for cross-platform consistency
    node_path = node.get_path()
    build_env = node.get_build_env()
    # Get the unique build environment context for this file node
    if re.search(r'/proto/(lib|src)/', node_path):
        # Remove -fno-rtti from this file's flags if it exists
        if build_env and "CXXFLAGS" in build_env:
            build_env["CXXFLAGS"] = [flag for flag in build_env["CXXFLAGS"] if flag != "-fno-rtti"]

        # Explicitly append -frtti for this file only
        build_env.Append(CXXFLAGS=["-frtti"])
    return node

# Register the middleware to scan each file node during construction
env.AddBuildMiddleware(enable_rtti_for_file)
