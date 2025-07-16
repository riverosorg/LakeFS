# SPDX-FileCopyrightText: 2025 Caleb Depatie
#
# SPDX-License-Identifier: 0BSD

# For local use to make it easy to install/replace/deinstall lakefs

PORTNAME=LakeFS
DISTVERSION=0.2.1
CATEGORIES=sysutils
MASTER_SITES=https://github.com/riverosorg/LakeFS/archive/refs/tags/
DISTNAME=v${DISTVERSION}
EXTRACT_SUFX=.zip

MAINTAINER=youremail@example.com
COMMENT=tag based file system
WWW=https://github.com/riverosorg/LakeFS

USES=7z meson

USE_GCC=yes

COMMON_DEPENDS= ${LOCALBASE}/include/argparse/argparse.hpp:devel/argparse 
				
LIB_DEPENDS= libspdlog.so:devel/spdlog \
			 libfuse3.so:filesystems/fusefs-libs3 \
			 libfmt.so:devel/libfmt \
			 libphobos2-ldc-shared.so:lang/ldc \
			 libdruntime-ldc-shared.so:lang/ldc \

BUILD_DEPENDS= meson:devel/meson \
			   ninja:devel/ninja \
			   ldc2:lang/ldc \
			   rst2man:textproc/py-docutils \
			   ${COMMON_DEPENDS}

RUN_DEPENDS= ${COMMON_DEPENDS}

WRKSRC=${WRKDIR}/${PORTNAME}-${DISTVERSION}

do-configure:
	meson setup ${WRKSRC} ${WRKSRC}/${MESON_BUILD_DIR}

post-stage:	
	cp ${WRKSRC}/${MESON_BUILD_DIR}/docs/*.1.gz ${STAGEDIR}/${PREFIX}/man/man1/

	cp ${WRKSRC}/lakefs.conf ${STAGEDIR}/${PREFIX}/etc/lakefs.conf.sample

.include <bsd.port.mk>

