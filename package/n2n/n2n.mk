####################################################
# N2N package for Edgepbx FX02/FX08/FX12/ED1 IPPBX #
#                 www.edgepbx.cn                   #
#              LI YUQIAN <yu@edgepbx.cn>           #
####################################################

N2N_DIR=$(DL_DIR)/n2n
N2N_SOURCE=n2n
N2N_SITE=https://svn.ntop.org/svn/ntop/trunk/n2n
N2N_VERSION=3869
N2N_WORKING=$(BUILD_DIR)/$(N2N_SOURCE)


$(DL_DIR)/$(N2N_SOURCE):
	$(SVN) $(N2N_SITE) --revision $(N2N_VERSION) $(DL_DIR)/$(N2N_SOURCE)
	-find $(DL_DIR)/$(N2N_SOURCE) -type d -name .svn | xargs rm -rf

$(N2N_WORKING)/.unpacked: $(DL_DIR)/$(N2N_SOURCE)
	cp -Rf $(DL_DIR)/$(N2N_SOURCE) $(N2N_WORKING)
	touch $(N2N_WORKING)/.unpacked

$(N2N_WORKING)/.configured: $(N2N_WORKING)/.unpacked
	$(PATCH_KERNEL) $(N2N_WORKING) package/n2n n2n.patch
	touch $(N2N_WORKING)/.configured

n2n: $(N2N_WORKING)/.configured
	make -C $(N2N_WORKING)
	cp -f $(N2N_WORKING)/edge $(TARGET_DIR)/usr/sbin/ 
	cp -f $(N2N_WORKING)/supernode $(TARGET_DIR)/usr/bin/ 

n2n-clean:
	rm -rf $(N2N_WORKING)
n2n-dirclean:
	rm -rf $(N2N_WORKING)

ifeq ($(strip $(SF_PACKAGE_N2N)),y)
n2n_: n2n
else
n2n_:
	rm -f $(TARGET_DIR)/usr/sbin/edge
	rm -f $(TARGET_DIR)/usr/bin/supernode
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=n2n_
