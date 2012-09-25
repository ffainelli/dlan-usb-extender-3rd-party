#
# Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wmbus-repeater
PKG_RELEASE:=3

include $(INCLUDE_DIR)/package.mk

define Package/wmbus-repeater
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_mcs814x
  TITLE:=Wireless metering repeater
  MAINTAINER:=Florian Fainelli <florian@openwrt.org>
endef

define Package/wmbus-repeater/description
  Wireless metering repeater
endef

define Build/Prepare
	$(INSTALL_DIR) $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)"
endef

define Package/wmbus-repeater/install
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/wmbus-repeater.config $(1)/etc/config/wmbus-repeater
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/wmbus-repeater.init $(1)/etc/init.d/wmbus-repeater
	$(INSTALL_DIR) $(1)/etc/hotplug.d/tty
	$(INSTALL_DATA) ./files/wmbus-repeater.hotplug $(1)/etc/hotplug.d/tty/30-wmbus-repeater
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/wmbus-repeater $(1)/usr/bin/wmbus-repeater
endef

$(eval $(call BuildPackage,wmbus-repeater))
