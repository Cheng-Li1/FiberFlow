// This file is designed for Zig build system 0.12
const std = @import("std");

pub fn build(b: *std.Build) void {
    // It should be default to debug if not specified
    const optimize = b.standardOptimizeOption(.{});

    // Change the target platform here if needed
    const target = b.resolveTargetQuery(std.zig.CrossTarget{
        .cpu_arch = .aarch64,
        .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_a53 },
        .os_tag = .freestanding,
        .abi = .none,
        .ofmt = .elf,
    });

    const lib = b.addStaticLibrary(.{
        .name = "FiberFlow",
        .target = target,
        .optimize = optimize,
        // For library, we should always maintain the Symbol Table
        .strip = false,
    });

    // Add include directory
    lib.addIncludePath(.{ .path = "." });

    // Add any additional compiler flags if needed
    lib.addCSourceFile(.{ .file = b.path("FiberFlow.c") });

    b.installArtifact(lib);
}
