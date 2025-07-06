<!--
SPDX-FileCopyrightText: 2024-2025 Caleb Depatie

SPDX-License-Identifier: CC-BY-4.0
-->

# LakeFS

A tag based filesystem providing enhanced querying and piping capabilities, while remaining compatible with traditional file managers.

## Querying

The query language is made of a few simple infix operators, which can operate on tags or subqueries.
Queries are enclosed by parentheses, and are composed of tags, subqueries, and operators.

The operators are:

- **&** - Logical AND, a file needs both right hand side and left.
- **|** - Logical OR, a file needs either right hand side or left.
- **!** - Logical NOT, a file cannot have the tag or query.

A few examples of queries are:

- **(tag1 & tag2)** - This will return all files that have both tag1 and tag2.
- **(tag1 | tag2)** - This will return all files that have either tag1 or tag2.
- **(tag1 & (tag2 | tag3))** - This will return all files that have tag1 and either tag2 or tag3.

## Installing

Packaging available and tested for RHEL and FreeBSD systems. Nix Flake untested.

---

This project is [REUSE Compliant](https://reuse.software/)