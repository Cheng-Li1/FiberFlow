const std = @import("std");

pub fn build(b: *std.Build) void {
    const optimize = b.standardOptimizeOption(.{});

    // Change the target platform here if needed
    const target = b.resolveTargetQuery(std.zig.CrossTarget{
        .cpu_arch = .aarch64,
        .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_a53 },
        .os_tag = .freestanding,
        .abi = .none,
    });

    const lib = b.addStaticLibrary(.{
        .name = "FiberFlow.a",
        .target = target,
        .optimize = optimize,
        .strip = false,
    });

    // Add include directory
    lib.addIncludePath(.{ .path = "." });

    // Add any additional compiler flags if needed
    lib.addCSourceFile(.{ .file = b.path("FiberFlow.c") });

    b.installArtifact(lib);
}
