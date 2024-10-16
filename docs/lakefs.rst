.. SPDX-FileCopyrightText: 2024 Caleb Depatie
..
.. SPDX-License-Identifier: CC-BY-4.0

.. Man page for the LakeFS command

======
lakefs
======

------------------------------
Initialize a a LakeFS instance
------------------------------

:Author: Caleb Depatie
:Author: Conner Tenn
:Copyright: CC-BY-4.0
:Version: 0.1.0
:Manual section: 1
:Manual group: LakeFS

SYNOPSIS
========

| lakefs [*OPTION*]... *mount_point*


DESCRIPTION
===========

LakeFS provides a tag based overlay for your filesystem.
Tags and files can be added to tracking using the **lakefs-cli(1)** tool.
This command will initialize a LakeFS instance at the given mount point.

QUERYING
--------

The query language is made of a few simple infix operators, which can operate on tags or subqueries.
Queries are enclosed by parentheses, and are composed of tags, subqueries, and operators.

The operators are:

- **&** - Logical AND, a file needs both right hand side and left.
- **|** - Logical OR, a file needs either right hand side or left.
- **!** - Logical NOT, a file cannot have the tag or query.

QUERY EXAMPLES
--------------

A few examples of queries are:

- **(tag1 & tag2)** - This will return all files that have both tag1 and tag2.
- **(tag1 | tag2)** - This will return all files that have either tag1 or tag2.
- **(tag1 & (tag2 | tag3))** - This will return all files that have tag1 and either tag2 or tag3.

OPTIONS
=======

mount_point
    Folder to mount the LakeFS instance under.

-v, --version
    Display version information and exit.

-h, --help
    Display help information and exit.


SEE ALSO
========

**lakefs-cli(1)** **lakefs.conf(5)**