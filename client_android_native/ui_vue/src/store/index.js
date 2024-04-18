import {
    createStore
} from 'vuex'
import Native from '../utils/native'
import { Toast } from 'vant';
import { createI18n } from 'vue-i18n';
// 默认中文
const lang = 'zh'
const i18n = new createI18n({
    locale: localStorage.getItem("lang") || lang ,// 语言标识（缓存里面没有就用中文）
    fallbackLocale: lang, //没有英文的时候默认中文语言
    globalInjection: true,
    legacy: false,
    messages: {
        'zh': require('@/utils/i18n/zh.json') ,
        'en': require('@/utils/i18n/en.json'),
        'tw': require('@/utils/i18n/tw.json'),
        // 如果还有新的语言在下面继续添加
    }
})
const { t } = i18n.global;
export default createStore({
    state: {
        // Host: "https://cloudlark.pingxingyun.com:8182/",
        // Host: "https://cloudlark.pingxingyun.com:8180/",
        // Host: "http://192.168.0.55:8181/",
        // Host: "http://222.128.6.137:8585/",
        Host: "",
        // Host: "https://cloudlark.pingxingyun.com:8180/",
        SrHost: "",

        hasPermission: false,
        selectedArSDKType: Native.AR_SDK_TYPE_ARCORE,
        arcoreSupportStatus: -1,
        arcoreInstallStatus: -1,
        arengineSupportStatus: -1,
        arengineInstallStatus: -1,

        // 2 == fast, 3 == normal, 4 == extreme
        quickConfigLevel: Native.QuickConfigLevel_Fast,
    },
    getters: {
        isArcoreReady(state) {
            return (state.arcoreSupportStatus == 0 || state.arcoreSupportStatus == 1) && state.arcoreInstallStatus == 0;
        },
        isArengineReady(state) {
            return (state.arengineSupportStatus == 0 || state.arengineSupportStatus == 1) && state.arengineInstallStatus == 0;
        },
        isARReady(state, getters) {
            return state.hasPermission && (getters.isArcoreReady || getters.isArengineReady)
        },
        selectedArSDKString(state) {
            return state.selectedArSDKType + "";
        },
        arcoreSupportStatusText({
            arcoreSupportStatus
        }) {
            switch (arcoreSupportStatus) {
                case 0:
                    /*支持*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "支持";
                        case 'en':
                            return   "supported";
                        case 'tw':
                            return  "支持";
                        default:
                            return  "支持";
                    };
                   /* return t("message.setUpPage.arcoreSupportStatus0");*/
                case 1:
                    /*支持但需要更新状态*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "支持但需要更新状态";
                        case 'en':
                            return   "Supported but requires status update";
                        case 'tw':
                            return  "支持但需要更新狀態";
                        default:
                            return  "支持但需要更新状态";
                    };
                  /*  return t("message.setUpPage.arcoreSupportStatus1");*/
                case 2:
                    /*不支持*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "不支持";
                        case 'en':
                            return   "Not supported";
                        case 'tw':
                            return  "不支持";
                        default:
                            return  "不支持";
                    };
                   /* return t("message.setUpPage.arcoreSupportStatus2");*/
            }
            /*'未知是否支持'*/
            switch (localStorage.getItem("lang")) {
                case 'zh':
                    return   "未知是否支持";
                case 'en':
                    return   "Unknown if supported";
                case 'tw':
                    return  "未知是否支持";
                default:
                    return  "未知是否支持";
            };

            /*return t("message.setUpPage.arcoreSupportStatus3");*/
        },
        arcoreInstallStatusText({
            arcoreInstallStatus
        }) {
            switch (arcoreInstallStatus) {
                case 0:
                    /*已安装*/
                   /* return t("message.setUpPage.arcoreInstallStatus0");*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "已安装";
                        case 'en':
                            return   "already installed";
                        case 'tw':
                            return  "已安裝";
                        default:
                            return  "已安裝";
                    };
                case 1:
                    /*需要安装*/
                    /*return  t("message.setUpPage.arcoreInstallStatus1");*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "需要安装";
                        case 'en':
                            return   "Installation required";
                        case 'tw':
                            return  "需要安裝";
                        default:
                            return  "需要安装";
                    };
                case 2:
                   /* 不支持*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "不支持";
                        case 'en':
                            return   "Not supported";
                        case 'tw':
                            return  "不支持";
                        default:
                            return  "不支持";
                    };
                   /* return  t("message.setUpPage.arcoreInstallStatus2");*/
            }
            /*未知是否安装*/
         /*   return  t("message.setUpPage.arcoreInstallStatus3");*/
            switch (localStorage.getItem("lang")) {
                case 'zh':
                    return   "未知是否安装";
                case 'en':
                    return   "Unknown whether to install or not";
                case 'tw':
                    return  "未知是否安裝";
                default:
                    return  "未知是否安装";
            };
        },
        arcoreStatusReason(state) {
            if (state.arcoreSupportStatus == -1) {
                switch (localStorage.getItem("lang")) {
                    case 'zh':
                        return   "未知环境";
                    case 'en':
                        return   "Unknown environment";
                    case 'tw':
                        return  "未知環境";
                    default:
                        return  "未知环境";
                };

            } else if (state.arcoreSupportStatus == 2) {
                /*不支持*/
                switch (localStorage.getItem("lang")) {
                    case 'zh':
                        return   "不支持";
                    case 'en':
                        return   "UnSupport";
                    case 'tw':
                        return  "不支持";
                    default:
                        return  "不支持";
                };
            } else {
                if (state.arcoreInstallStatus == 0) {
                    if (state.arcoreSupportStatus == 1) {
                        /*已就绪,但需要更新状态*/
                        switch (localStorage.getItem("lang")) {
                            case 'zh':
                                return   "已就绪,但需要更新状态";
                            case 'en':
                                return   "Ready, but status needs to be updated";
                            case 'tw':
                                return  "已就緒，但需要更新狀態";
                            default:
                                return  "已就绪,但需要更新状态";
                        };
                    } else {
                        /*已就绪*/
                        switch (localStorage.getItem("lang")) {
                            case 'zh':
                                return   "已就绪";
                            case 'en':
                                return   "Ready";
                            case 'tw':
                                return  "已就緒";
                            default:
                                return  "已就绪";
                        };
                    }
                } else if (state.arcoreInstallStatus == 1) {
                    /* 需要安装环境*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "需要安装环境";
                        case 'en':
                            return   "Installation environment required";
                        case 'tw':
                            return  "需要安裝環境";
                        default:
                            return  "需要安装环境";
                    };
                } else {
                    /* 安装失败*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "安装失败";
                        case 'en':
                            return   "Installation failed";
                        case 'tw':
                            return  "安裝失敗";
                        default:
                            return  "安装失败";
                    };
                }
            }
        /*    if (state.arcoreSupportStatus == -1) {
                /!*未知环境*!/
                return  t("message.setUpPage.arcoreStatusReason0");
            } else if (state.arcoreSupportStatus == 2) {
                /!*不支持*!/
                return t("message.setUpPage.arcoreStatusReason1");
            } else {
                if (state.arcoreInstallStatus == 0) {
                    if (state.arcoreSupportStatus == 1) {
                        /!*已就绪,但需要更新状态*!/
                        return t("message.setUpPage.arcoreStatusReason2");
                    } else {
                        /!*已就绪*!/
                        return t("message.setUpPage.arcoreStatusReason3");
                    }
                } else if (state.arcoreInstallStatus == 1) {
                   /!* 需要安装环境*!/
                    return t("message.setUpPage.arcoreStatusReason4");
                } else {
                   /!* 安装失败*!/
                    return t("message.setUpPage.arcoreStatusReason5");
                }
            }*/
        },
        arengineSupportStatusText({
            arengineSupportStatus
        }) {
            switch (arengineSupportStatus) {
                case 0:
                    /*支持*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "支持";
                        case 'en':
                            return   "supported";
                        case 'tw':
                            return  "支持";
                        default:
                            return  "支持";
                    };
                /* return t("message.setUpPage.arcoreSupportStatus0");*/
                case 1:
                    /*支持但需要更新状态*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "支持但需要更新状态";
                        case 'en':
                            return   "Supported but requires status update";
                        case 'tw':
                            return  "支持但需要更新狀態";
                        default:
                            return  "支持但需要更新状态";
                    };
                /*  return t("message.setUpPage.arcoreSupportStatus1");*/
                case 2:
                    /*不支持*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "不支持";
                        case 'en':
                            return   "Not supported";
                        case 'tw':
                            return  "不支持";
                        default:
                            return  "不支持";
                    };
                /* return t("message.setUpPage.arcoreSupportStatus2");*/
            }
            /*未知是否安装*/
            switch (localStorage.getItem("lang")) {
                case 'zh':
                    return   "未知是否支持";
                case 'en':
                    return   "Unknown if supported";
                case 'tw':
                    return  "未知是否支持";
                default:
                    return  "未知是否支持";
            };
        },
        arengineInstallStatusText({
            arengineInstallStatus
        }) {
            switch (arengineInstallStatus) {
                case 0:
                    /*已安装*/
                    /* return t("message.setUpPage.arcoreInstallStatus0");*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "已安装";
                        case 'en':
                            return   "already installed";
                        case 'tw':
                            return  "已安裝";
                        default:
                            return  "已安裝";
                    };
                case 1:
                    /*需要安装*/
                    /*return  t("message.setUpPage.arcoreInstallStatus1");*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "需要安装";
                        case 'en':
                            return   "Installation required";
                        case 'tw':
                            return  "需要安裝";
                        default:
                            return  "需要安装";
                    };
                case 2:
                    /* 不支持*/
                    switch (localStorage.getItem("lang")) {
                        case 'zh':
                            return   "不支持";
                        case 'en':
                            return   "Not supported";
                        case 'tw':
                            return  "不支持";
                        default:
                            return  "不支持";
                    };
            }
            /*未知是否安装*/
            switch (localStorage.getItem("lang")) {
                case 'zh':
                    return   "未知是否安装";
                case 'en':
                    return   "Unknown whether to install or not";
                case 'tw':
                    return  "未知是否安裝";
                default:
                    return  "未知是否安装";
            };
        },
        arengineStatusReason(state) {
              if (state.arengineSupportStatus == -1) {
                      switch (localStorage.getItem("lang")) {
                          case 'zh':
                              return   "未知环境";
                          case 'en':
                              return   "Unknown environment";
                          case 'tw':
                              return  "未知環境";
                          default:
                              return  "未知环境";
                      };

          } else if (state.arengineSupportStatus == 2) {
              /*不支持*/
                  switch (localStorage.getItem("lang")) {
                      case 'zh':
                          return   "不支持";
                      case 'en':
                          return   "UnSupport";
                      case 'tw':
                          return  "不支持";
                      default:
                          return  "不支持";
                  };
          } else {
              if (state.arengineInstallStatus == 0) {
                  if (state.arengineSupportStatus == 1) {
                      /*已就绪,但需要更新状态*/
                      switch (localStorage.getItem("lang")) {
                          case 'zh':
                              return   "已就绪,但需要更新状态";
                          case 'en':
                              return   "Ready, but status needs to be updated";
                          case 'tw':
                              return  "已就緒，但需要更新狀態";
                          default:
                              return  "已就绪,但需要更新状态";
                      };
                  } else {
                      /*已就绪*/
                      switch (localStorage.getItem("lang")) {
                          case 'zh':
                              return   "已就绪";
                          case 'en':
                              return   "Ready";
                          case 'tw':
                              return  "已就緒";
                          default:
                              return  "已就绪";
                      };
                  }
              } else if (state.arengineInstallStatus == 1) {
                  /* 需要安装环境*/
                  switch (localStorage.getItem("lang")) {
                      case 'zh':
                          return   "需要安装环境";
                      case 'en':
                          return   "Installation environment required";
                      case 'tw':
                          return  "需要安裝環境";
                      default:
                          return  "需要安装环境";
                  };
              } else {
                  /* 安装失败*/
                  switch (localStorage.getItem("lang")) {
                      case 'zh':
                          return   "安装失败";
                      case 'en':
                          return   "Installation failed";
                      case 'tw':
                          return  "安裝失敗";
                      default:
                          return  "安装失败";
                  };
              }
          }
           // 这种实现方式不支持页面在切换语言时立即刷新，因此我们采用了上述相对笨拙的方法。
          /*  if (state.arengineSupportStatus == -1) {
                /!*未知环境*!/
                return  t("message.setUpPage.arcoreStatusReason0");
            } else if (state.arengineSupportStatus == 2) {
                /!*不支持*!/
                return t("message.setUpPage.arcoreStatusReason1");
            } else {
                if (state.arengineInstallStatus == 0) {
                    if (state.arengineSupportStatus == 1) {
                        /!*已就绪,但需要更新状态*!/
                        return t("message.setUpPage.arcoreStatusReason2");
                    } else {
                        /!*已就绪*!/
                        return t("message.setUpPage.arcoreStatusReason3");
                    }
                } else if (state.arengineInstallStatus == 1) {
                    /!* 需要安装环境*!/
                    return t("message.setUpPage.arcoreStatusReason4");
                } else {
                    /!* 安装失败*!/
                    return t("message.setUpPage.arcoreStatusReason5");
                }
            }*/
        },
        quickConfigLevelString(state) {
            return state.quickConfigLevel + "";
        }
    },
    mutations: {
        setHost(state, host) {
            state.Host = host;
        },
        setSrHost(state, host) {
            state.SrHost = host;
        },

        setSelectedArSDKType(state, type) {
            state.selectedArSDKType = type;
            console.log('setSelectedArSDKType', type);
        },

        setHasPermission(state, hasPermission) {
            state.hasPermission = hasPermission;
        },

        setArcoreSupportStatus(state, arcoreSupportStatus) {
            state.arcoreSupportStatus = arcoreSupportStatus;
        },

        setArcoreInstallStatus(state, arcoreInstallStatus) {
            state.arcoreInstallStatus = arcoreInstallStatus;
        },

        setArengineSupportStatus(state, arengineSupportStatus) {
            state.arengineSupportStatus = arengineSupportStatus;
        },

        setArengineInstallStatus(state, arengineInstallStatus) {
            state.arengineInstallStatus = arengineInstallStatus;
        },

        setQuickConfigLevel(state, level) {
            state.quickConfigLevel = level;
        },
        freshLan(state,lan){
            window.localStorage.setItem('lang', lan);
            state.localLan = window.localStorage.getItem('lang') || 'zh';
           // i18n.global.locale = lan;

        },
    },
    actions: {
        // commit, state, getters, dispatch
        saveHost({
            commit
        }, {host, srHost}) {

            commit('setHost', host);

            if (srHost) {
                commit('setSrHost', srHost);
            } else {
                commit('setSrHost', host);
            }

            // 去空格
            host = host.trim();
            localStorage.setItem('host', host);

            if (srHost) {
                srHost = srHost.trim();
                localStorage.setItem('srHost', srHost);
            } else {
                localStorage.setItem('srHost', host);
            }

            try {
                const url = new URL(host);
                console.log('native saveserver address ', url, url.hostname, url.port);
                Native.onSaveServerAddress(url.hostname, url.port);
            } catch (e) {
                console.warn('parse host url failed', host);
            }
        },

        restoreHost({
            commit
        }) {
            let host = localStorage.getItem('host');
            if (host) {
                commit('setHost', host);
            }
            let srHost = localStorage.getItem('srHost');
            if (srHost) {
                commit('setSrHost', srHost);
            } else if (host) {
                commit('setSrHost', host);
            }

            try {
                const url = new URL(host);
                console.log('native saveserver address ', url, url.hostname, url.port);
                Native.onSaveServerAddress(url.hostname, url.port);
            } catch (e) {
                console.warn('parse host url failed', host);
            }
        },

        selectARSDK({
            commit
        }, type) {
            commit('setSelectedArSDKType', type);
            localStorage.setItem('sdkType', type);
            Native.selectARSDK(type);
        },

        restoreSDKType({
            commit
        }) {
            let sdkType = parseInt(localStorage.getItem('sdkType'));
            if (!sdkType) {
                // set to default
                commit('setSelectedArSDKType', Native.AR_SDK_TYPE_ARCORE);
                return;
            }
            commit('setSelectedArSDKType', sdkType);
            Native.selectARSDK(sdkType);
        },

        selectQuickConfigLevel({
            commit,
        }, level) {
            commit('setQuickConfigLevel', level);
            localStorage.setItem('quickConfigLevel', level);
            Native.selectQuickConfigLevel(parseInt(level));
        },

        restoreQuickConfigLevel({
            commit,
            dispatch,
        }) {
            let level = localStorage.getItem('quickConfigLevel', level);
            // save default
            if (!level) {
                dispatch('selectQuickConfigLevel', Native.QuickConfigLevel_Fast);
                Native.selectQuickConfigLevel(Native.QuickConfigLevel_Fast);
                return;
            }
            commit('setQuickConfigLevel', parseInt(level));
            Native.selectQuickConfigLevel(parseInt(level));
        },

        updateStatus({
            state,
            commit
        }) {
            commit('setHasPermission', Native.hasPermission());

            commit('setArcoreSupportStatus', Native.arcoreSupportStatus());
            // 支持时检测安装
            if (state.arcoreSupportStatus == 0 || state.arcoreSupportStatus == 1) {
                commit('setArcoreInstallStatus', Native.arcoreInstallStatus(false));
            }

            commit('setArengineSupportStatus', Native.hwarengineSupportStatus());

            if (state.arengineSupportStatus == 0 || state.arengineSupportStatus == 1) {
                commit('setArengineInstallStatus', Native.hwarengineInstallStatus(false));
            }
        },

        checkHasPersission({
            state,
            commit
        }) {
            commit('setHasPermission', Native.hasPermission());
            if (state.hasPermission) {
                Toast("已获得权限");
            } else {
                Toast("请授予权限以正常使用AR功能");
                Native.checkPermission();
            }
        },

        checkArcoreSupport({
            commit,
            getters
        }) {
            commit('setArcoreSupportStatus', Native.arcoreSupportStatus());
            Toast(getters.arcoreSupportStatusText);
        },

        checkArcoreInstall({
            commit,
            getters,
        }, require) {
            commit('setArcoreInstallStatus', Native.arcoreInstallStatus(require));
            Toast(getters.arcoreInstallStatusText);
        },

        checkArengineSupport({
            commit,
            getters,
        }) {
            commit('setArengineSupportStatus', Native.hwarengineSupportStatus());
            Toast(getters.arengineSupportStatusText);
        },

        checkArengineInstall({
            commit,
            getters,
        }, require) {
            commit('setArengineInstallStatus', Native.hwarengineInstallStatus(require));
            Toast(getters.arengineInstallStatusText);
        },
    },
/*    setup() {
        const { t } = useI18n();     //解析出t方法
        const message.setUpPage = t("message.setUpPage");
        return { message.setUpPage } //可在js中打印,vue2中可用this
    }*/
})
