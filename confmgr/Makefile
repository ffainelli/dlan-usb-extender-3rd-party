#
# Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
#

include $(TOPDIR)/rules.mk

PKG_NAME:=confmgr
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk


define Package/confmgr
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=@TARGET_mcs814x
  TITLE:=Configuration manager utility
  MAINTAINER:=Florian Fainelli <florian@openwrt.org>
endef

define Package/confmgr/description
  Configuration manager utility
endef


define Build/Prepare
endef

define Build/Configure
endef

define Build/Compile
endef


define Package/confmgr/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) files/confmgr.init $(1)/etc/init.d/confmgr
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) files/confmgr.sh $(1)/bin/confmgr
endef

$(eval $(call BuildPackage,confmgr))
