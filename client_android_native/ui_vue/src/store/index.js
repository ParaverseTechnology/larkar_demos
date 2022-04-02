import {
    createStore
} from 'vuex'
import Native from '../utils/native'
import { Toast } from 'vant';

export default createStore({
    state: {
        // Host: "https://cloudlark.pingxingyun.com:8182/",
        // Host: "https://cloudlark.pingxingyun.com:8180/",
        // Host: "http://192.168.0.55:8181/",
        // Host: "http://222.128.6.137:8585/",
        Host: "",

        hasPermission: false,
        selectedArSDKType: Native.AR_SDK_TYPE_ARCORE,
        arcoreSupportStatus: -1,
        arcoreInstallStatus: -1,
        arengineSupportStatus: -1,
        arengineInstallStatus: -1,
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
                    return '支持';
                case 1:
                    return '支持但需要更新状态';
                case 2:
                    return '不支持';
            }
            return '未知是否支持';
        },
        arcoreInstallStatusText({
            arcoreInstallStatus
        }) {
            switch (arcoreInstallStatus) {
                case 0:
                    return '已安装';
                case 1:
                    return '需要安装';
                case 2:
                    return '不支持';
            }
            return '未知是否安装';
        },
        arcoreStatusReason(state) {
            if (state.arcoreSupportStatus == -1) {
                return "未知环境";
            } else if (state.arcoreSupportStatus == 2) {
                return '不支持';
            } else {
                if (state.arcoreInstallStatus == 0) {
                    if (state.arcoreSupportStatus == 1) {
                        return '已就绪,但需要更新状态';
                    } else {
                        return '已就绪';
                    }
                } else if (state.arcoreInstallStatus == 1) {
                    return '需要安装环境';
                } else {
                    return '安装失败';
                }
            }
        },
        arengineSupportStatusText({
            arengineSupportStatus
        }) {
            switch (arengineSupportStatus) {
                case 0:
                    return '支持';
                case 1:
                    return '支持但需要更新状态';
                case 2:
                    return '不支持';
            }
            return '未知是否支持';
        },
        arengineInstallStatusText({
            arengineInstallStatus
        }) {
            switch (arengineInstallStatus) {
                case 0:
                    return '已安装';
                case 1:
                    return '需要安装';
                case 2:
                    return '不支持';
            }
            return '未知是否安装';
        },
        arengineStatusReason(state) {
            if (state.arengineSupportStatus == -1) {
                return "未知环境";
            } else if (state.arengineSupportStatus == 2) {
                return '不支持';
            } else {
                if (state.arengineInstallStatus == 0) {
                    if (state.arengineSupportStatus == 1) {
                        return '已就绪,但需要更新状态';
                    } else {
                        return '已就绪';
                    }
                } else if (state.arengineInstallStatus == 1) {
                    return '需要安装环境';
                } else {
                    return '安装失败';
                }
            }
        }
    },
    mutations: {
        setHost(state, host) {
            state.Host = host;
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
        }
    },
    actions: {
        // commit, state, getters, dispatch
        saveHost({
            commit
        }, host) {
            commit('setHost', host);
            localStorage.setItem('host', host);
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

        updateStatus({
            commit
        }) {
            commit('setHasPermission', Native.hasPermission());

            commit('setArcoreSupportStatus', Native.arcoreSupportStatus());
            commit('setArcoreInstallStatus', Native.arcoreInstallStatus(false));

            commit('setArengineSupportStatus', Native.hwarengineSupportStatus());
            commit('setArengineInstallStatus', Native.hwarengineInstallStatus(false));
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
})