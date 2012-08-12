#
# Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
#
# This is proprietary software
#

include $(TOPDIR)/rules.mk

PKG_NAME:=hpav-firmware
PKG_RELEASE:=1
PKG_VERSION:=4.4.0-02-NW6__-5-X-FINAL

include $(INCLUDE_DIR)/package.mk

define Package/hpav-firmware
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_mcs814x
  TITLE:=HomePlug AV firmwares
  MAINTAINER:=Florian Fainelli <florian@openwrt.org>
endef

define Package/hpav-firmware/description
  This package contains the INT6x00 PIB and firmware files.
endef


define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef

define Package/hpav-firmware/install
	$(INSTALL_DIR) $(1)/firmware/
	$(INSTALL_BIN) files/* $(1)/firmware/
endef

$(eval $(call BuildPackage,hpav-firmware))
