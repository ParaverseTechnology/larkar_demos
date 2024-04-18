import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import Vant from 'vant';
import 'vant/lib/index.css';
import store from './store'
import { mapState, mapGetters, mapMutations, mapActions } from "vuex";
import i18n from './utils/i18n/i18n.js';

const mixin = {
    computed: {
        ...mapState({
            Host: state => state.Host,
            SrHost: state => state.SrHost,
            selectedArSDKType: state => state.selectedArSDKType,
            hasPermission: state => state.hasPermission,
            arcoreSupportStatus: state => state.arcoreSupportStatus,
            arcoreInstallStatus: state => state.arcoreInstallStatus,
            arengineSupportStatus: state => state.arengineSupportStatus,
            arengineInstallStatus: state => state.arengineInstallStatus,
            quickConfigLevel: state => state.quickConfigLevel,
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
            quickConfigLevelString: 'quickConfigLevelString',
        }),
    },
    methods: {
        ...mapMutations({}),
        ...mapActions({
            saveHost: 'saveHost',
            restoreHost: 'restoreHost',
            updateStatus: 'updateStatus',
            checkHasPersission: 'checkHasPersission',
            checkArcoreSupport: 'checkArcoreSupport',
            checkArcoreInstall: 'checkArcoreInstall',
            checkArengineSupport: 'checkArengineSupport',
            checkArengineInstall: 'checkArengineInstall',
            selectARSDK: 'selectARSDK',
            restoreSDKType: 'restoreSDKType',
            selectQuickConfigLevel: 'selectQuickConfigLevel',
            restoreQuickConfigLevel: 'restoreQuickConfigLevel',
        }),
    }
}


/*//多语言国际化
import { createI18n } from 'vue-i18n'
import en from './utils/i18n/en.json'
import cn from './utils/i18n/zh.json'
import tw from './utils/i18n/tw.json'


// 获取浏览器语言
const i18n = new createI18n({
    locale: localStorage.getItem("lang") || 'zh' ,// 语言标识（缓存里面没有就用中文）
    fallbackLocale: 'zh', //没有英文的时候默认中文语言
    /!*globalInjection: true,*!/
    legacy: false,
    messages: {
        'zh': cn ,
        'en': en,
        'tw': tw,
        // 如果还有新的语言在下面继续添加
    }
})

app.config.globalProperties.$multiLang=i18n*/
const app = createApp(App)
app
.use(store)
    .use(i18n)
.use(router)
.use(Vant)
.mixin(mixin)
.mount('#app')
