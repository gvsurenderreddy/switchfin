####################################################
# N2N package for Edgepbx FX02/FX08/FX12/ED1 IPPBX #
#                 www.edgepbx.cn                   #
#              LI YUQIAN <yu@edgepbx.cn>           #
####################################################

ifeq ($(strip $(SF_N2N_V2)),y)
	N2N_VERSION=2
else
	N2N_VERSION=1
endif

N2N_SITE=https://svn.ntop.org/svn/ntop/trunk/n2n
N2N_REVISION=4661
N2N_DIR=$(DL_DIR)/n2n
N2N_SUBDIR=n2n_v$(N2N_VERSION)
N2N_WORKING=$(BUILD_DIR)/$(N2N_SUBDIR)


$(N2N_DIR):
	$(SVN) $(N2N_SITE) --revision $(N2N_REVISION) $(N2N_DIR)

$(N2N_WORKING)/.unpacked: $(N2N_DIR)
	rm -rf $(N2N_WORKING)
	cp -R $(N2N_DIR)/$(N2N_SUBDIR) $(N2N_WORKING)
	touch $(N2N_WORKING)/.unpacked

$(N2N_WORKING)/.configured: $(N2N_WORKING)/.unpacked
	-find $(N2N_WORKING) -type d -name .svn | xargs rm -rf
	touch $(N2N_WORKING)/.configured

n2n: $(N2N_WORKING)/.configured
	CFLAGS="-O3 $(TARGET_CFLAGS) -I$(STAGING_INC) -L$(STAGING_LIB)" make -C $(N2N_WORKING) CC=$(TARGET_CC) DEBUG= edge supernode
	cp -f $(N2N_WORKING)/edge $(N2N_WORKING)/supernode $(TARGET_DIR)/usr/sbin/

n2n-clean n2n-dirclean:
	rm -rf $(BUILD_DIR)/n2n_v[12]

ifeq ($(strip $(SF_PACKAGE_N2N)),y)
n2n_: n2n
else
n2n_:
	rm -f $(TARGET_DIR)/usr/sbin/edge $(TARGET_DIR)/usr/sbin/supernode
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=n2n_
