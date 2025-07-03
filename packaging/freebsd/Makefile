# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# For local use to make it easy to install/replace/deinstall lakefs

PORTNAME=LakeFS
DISTVERSION=0.1.0
CATEGORIES=sysutils
MASTER_SITES=https://github.com/riverosorg/LakeFS/archive/refs/tags/
DISTNAME=v${DISTVERSION}
EXTRACT_SUFX=.zip

MAINTAINER=youremail@example.com
COMMENT=tag based file system
WWW=https://github.com/riverosorg/LakeFS

USES=7z meson

COMMON_DEPENDS= ${LOCALBASE}/include/argparse/argparse.hpp:devel/argparse 
				
LIB_DEPENDS= libspdlog.so:devel/spdlog

BUILD_DEPENDS= meson:devel/meson \
			   ninja:devel/ninja \
			   gcc:lang/gcc \
			   ldc2:lang/ldc \
			   rst2man:textproc/py-docutils \
			   ${COMMON_DEPENDS}

RUN_DEPENDS= ${COMMON_DEPENDS}

WRKSRC=${WRKDIR}/${PORTNAME}-${DISTVERSION}

do-configure:
	meson setup ${WRKSRC} ${WRKSRC}/${MESON_BUILD_DIR}

.include <bsd.port.mk>

