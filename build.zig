// SPDX-FileCopyrightText: 2024 Caleb Depatie
//
// SPDX-License-Identifier: 0BSD

const std = @import("std");
const builtin = std.builtin;

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const out = b.addExecutable(.{
        .name = "lakefs",
        .root_source_file = .{ .path = "src/zlake.zig" },
        .target = target,
        .optimize = optimize,
    });

    b.installArtifact(out);

    const run_exe = b.addRunArtifact(out);

    const run_step = b.step("run", "Run the application");
    run_step.dependOn(&run_exe.step);
}
