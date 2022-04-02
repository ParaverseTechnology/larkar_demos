import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import Vant from 'vant';
import 'vant/lib/index.css';
import store from './store'
import { mapState, mapGetters, mapMutations, mapActions } from "vuex";

const mixin = {
    computed: {
        ...mapState({
            Host: state => state.Host,
            selectedArSDKType: state => state.selectedArSDKType,
            hasPermission: state => state.hasPermission,
            arcoreSupportStatus: state => state.arcoreSupportStatus,
            arcoreInstallStatus: state => state.arcoreInstallStatus,
            arengineSupportStatus: state => state.arengineSupportStatus,
            arengineInstallStatus: state => state.arengineInstallStatus,
        }),
        ...mapGetters({
            isArcoreReady: 'isArcoreReady',
            isArengineReady: 'isArengineReady',
            isARReady: 'isARReady',
            arcoreStatusReason: 'arcoreStatusReason',
            arengineStatusReason: 'arengineStatusReason',
            arcoreSupportStatusText: 'arcoreSupportStatusText',
            arcoreInstallStatusText: 'arcoreInstallStatusText',
            arengineSupportStatusText: 'arengineSupportStatusText',
            arengineInstallStatusText: 'arengineInstallStatusText',
            selectedArSDKString: 'selectedArSDKString',
        }),
    },
    methods: {
        ...mapMutations({}),
        ...mapActions({
            saveHost: 'saveHost',
            restoreHost: 'restoreHost',
            updateStatus: 'updateStatus',
            checkArcoreSupport: 'checkArcoreSupport',
            checkArcoreInstall: 'checkArcoreInstall',
            checkArengineSupport: 'checkArengineSupport',
            checkArengineInstall: 'checkArengineInstall',
            selectARSDK: 'selectARSDK',
            restoreSDKType: 'restoreSDKType',
        }),
    }
}

createApp(App)
.use(store)
.use(router)
.use(Vant)
.mixin(mixin)
.mount('#app')
