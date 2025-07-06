.. SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
..
.. SPDX-License-Identifier: CC-BY-4.0

.. Man page for the LakeFS CTL

==========
lakefs-ctl
==========

-------------------------------------------------
Interact with the LakeFS tags and file references
-------------------------------------------------

:Author: Caleb Depatie
:Copyright: CC-BY-4.0
:Version: 0.2.0
:Manual section: 1
:Manual group: LakeFS

SYNOPSIS
========

| **lakefs-ctl help**
| **lakefs-ctl add** *PATH*
| **lakefs-ctl tag** *PATH TAG*
| **lakefs-ctl del** *PATH*
| **lakefs-ctl del-tag** *PATH TAG*
| **lakefs-ctl default** *QUERY*


DESCRIPTION
===========

**lakefs-ctl** provides a CLI for controlling a running LakeFS instance.
It gives LakeFS commands that will modify the database. Any **PATH** given should be relative to your current working directory.

**add** - Adds a file to tracking. No tags are added so this file will not be included in any queries yet.

**tag** - Adds a tag to a file. This will allow you to query for this file.

**del** - Removes a file from tracking. This will remove all tags associated with this file.

**del-tag** - Removes a single tag from a file.

**default** - Changes the default query at the LakeFS mount point. **QUERY** should be passed in quotes.


SEE ALSO
========

**lakefs(1)** **lakefs.conf(5)**