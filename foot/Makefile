#
# Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
#
# This is proprietary software
# Copyright (c) 2008 by devolo AG, Aachen, Germany

include $(TOPDIR)/rules.mk

PKG_NAME:=foot
PKG_RELEASE:=3

include $(INCLUDE_DIR)/package.mk

define Package/foot
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_mcs814x +hpav-firmware +libstdcpp +confmgr +hpavcfg
  TITLE:=INT6x00 Flashless boot Utility (foot)
  MAINTAINER:=Florian Fainelli <florian@openwrt.org>
endef

define Package/foot/description
  Intellon INT6x00 flashless boot utility
endef


define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef


define Package/foot/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) files/foot.init $(1)/etc/init.d/foot
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) files/foot.config $(1)/etc/config/foot
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) files/foot.elf $(1)/usr/bin/foot
endef

$(eval $(call BuildPackage,foot))
