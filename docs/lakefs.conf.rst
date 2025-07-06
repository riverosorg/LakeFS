.. SPDX-FileCopyrightText: 2024-2025 Caleb Depatie
..
.. SPDX-License-Identifier: CC-BY-4.0

.. Man page for the LakeFS command

===========
lakefs.conf
===========

-----------------------------------------
/etc/lakefs.conf configuration parameters
-----------------------------------------

:Author: Caleb Depatie
:Copyright: CC-BY-4.0
:Version: 0.2.0
:Manual section: 5
:Manual group: LakeFS

DESCRIPTION
===========

LakeFS uses a key-value interface for system configuration.
Each line of the file is a key-value pair separated by an equals sign and with no space between the equals and the key and value.

**dir** - Path to where the Database is stored. Backups will also be saved here


SEE ALSO
========

**lakefs-ctl(1)** **lakefs(1)**