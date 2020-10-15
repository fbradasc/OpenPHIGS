ADD_SUBDIRECTORY(include/phigs)

SET(P_ARCHIVE_SRCS
    archive/ar_conv.c
    archive/ar_ops.c
    archive/ar_hier.c
)

SET(P_CP_SRCS
    cp/cpb_pm.c
    cp/cpx_clnt.c
    cp/cpa_css.c
    cp/cpa_ws.c
    cp/cpm_xevnt.c
    cp/cpx_util.c
    cp/cpx_pm.c
    cp/cpb_util.c
    cp/cpb_clnt.c
    cp/cp_inp.c
    cp/cpx_ws.c
    cp/cpm_tlkt.c
    cp/cp_util.c
    cp/cpx_misc.c
    cp/psl.c
    cp/cpx_css.c
    cp/cpa_util.c
    cp/cpa_clnt.c
    cp/cpa_pm.c
    cp/cp_rcom.c
    cp/cpb_css.c
    cp/cp_ccom.c
    cp/cpx_ar.c
    cp/cpb_ws.c
)

SET(P_CSS_SRCS
    css/css_pr.c
    css/css_ini.c
    css/css_str.c
    css/css_inq.c
    css/css_ws.c
    css/css_el.c
    css/css_stb.c
    css/css_set.c
)

SET(P_C_BINDING_SRCS
    c_binding/cb_fill.c
    c_binding/cb_ws.c
    c_binding/cb_sedt.c
    c_binding/cb_colr.c
    c_binding/cb_ctrl.c
    c_binding/cb_err.c
    c_binding/cb_stru.c
    c_binding/cb_cell.c
    c_binding/cb_erfn.c
    c_binding/cb_plin.c
    c_binding/cb_lite.c
    c_binding/cb_nurb.c
    c_binding/cb_erhd.c
    c_binding/cb_dcue.c
    c_binding/cb_util.c
    c_binding/cb_nset.c
    c_binding/cb_xfut.c
    c_binding/cb_text.c
    c_binding/cb_esc.c
    c_binding/cb_wst.c
    c_binding/cb_ar.c
    c_binding/cb_pmrk.c
    c_binding/cb_mf.c
    c_binding/cb_inp.c
)

SET(P_ERROR_SRCS
    error/err_loc.c
    error/err_rmt.c
    error/error.c
)

SET(P_INPUT_SRCS
    input/sin_cho.c
    input/sin.c
    input/sin_dev.c
    input/sin_cvs.c
    input/sin_ws.c
    input/sin_strg.c
    input/sin_val.c
    input/sin_q.c
)

SET(P_PEX_SRCS
    pex/pex_ws.c
    pex/pex_stru.c
    pex/pex_lut.c
    pex/pex_srch.c
    pex/pex_pick.c
    pex/pex_font.c
    pex/pex_rend.c
    pex/pex_pipe.c
    pex/pex_ns.c
    pex/pex_info.c
    pex/pex_util.c
)

SET(P_SWAP_SRCS
    swap/swap_OCTables.c
    swap/swap_floatconv.c
    swap/swap_uOCprim.c
    swap/swap_cOCprim.c
    swap/swap_OCattr.c
    swap/swap_Convert.c
    swap/swap_OCcolour.c
)

SET(P_SW_TYPE_SRCS
    ws_type/wstx_atr.c
    ws_type/wstx_alst.c
    ws_type/wstx_ini.c
)

SET(P_UTIL_SRCS
    util/ut_misc.c
    util/ut_ntfy.c
    util/ut_vers.c
    util/ut_wchar.c
    util/utx_conv.c
    util/utx.c
    util/ut_path.c
    util/ut_mat.c
    util/ut_htab.c
)

SET(P_WS_SRCS
    ws/ws_inp.c
    ws/wsb.c
    ws/ws_pm.c
    ws/wsa_inp.c
    ws/wsb_pm.c
    ws/wsa_pm.c
    ws/wsa.c
    ws/wsx.c
    ws/wsx_util.c
    ws/wsx_lut.c
)

ADD_LIBRARY(phigs
            ${P_ARCHIVE_SRCS}
            ${P_CP_SRCS}
            ${P_CSS_SRCS}
            ${P_C_BINDING_SRCS}
            ${P_ERROR_SRCS}
            ${P_INPUT_SRCS}
            ${P_PEX_SRCS}
            ${P_SWAP_SRCS}
            ${P_SW_TYPE_SRCS}
            ${P_UTIL_SRCS}
            ${P_WS_SRCS}
)

ADD_EXECUTABLE(phigsmon
               phigsmon/phigsmon.c
)

TARGET_INCLUDE_DIRECTORIES(phigsmon
                           PRIVATE cp)

TARGET_LINK_LIBRARIES(phigsmon
                      ${PHIGS_LIBRARIES}
)

INSTALL(TARGETS
            phigsmon
            phigs
        RUNTIME DESTINATION lib/X11/PEX
        ARCHIVE DESTINATION lib
)
