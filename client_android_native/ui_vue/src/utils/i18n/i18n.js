// I18n
import { createI18n } from 'vue-i18n';


// 默认中文
const lang = 'zh'
const i18n = new createI18n({
    locale: localStorage.getItem("lang") || lang ,// 语言标识（缓存里面没有就用中文）
    fallbackLocale: lang, //没有英文的时候默认中文语言
    globalInjection: true,
    legacy: false,
    messages: {
        'zh': require('./zh.json') ,
        'en': require('./en.json'),
        'tw': require('./tw.json'),
        // 如果还有新的语言在下面继续添加
    }
})

export default i18n
