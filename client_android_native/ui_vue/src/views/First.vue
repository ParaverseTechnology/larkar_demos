<template>
    <div class="setup">
        <van-nav-bar
                :title="$t('message.setUpPage.title')">
            <template #left>
                <van-dropdown-menu>
                    <van-dropdown-item v-model="currentLan" :options="lanCategory" @change="onChangeLan"/>
                </van-dropdown-menu>
            </template>
        </van-nav-bar>
        <span style="position: absolute;opacity: 0">{{ dateTimeToFreshDom }}</span>
        <section class="section">
            <div class="block">
                <div class="block-title">
                    <!-- 进度-->
                    {{$t('process')}}
                </div>
                <van-steps :active="active">
                    <van-step>  {{$t("message.setUpPage.serverAddressTip")}}</van-step>
                    <van-step>  {{$t("message.setUpPage.check")}}AR SDK</van-step>
                    <van-step>{{$t("success")}}</van-step>
                </van-steps>
            </div>
            <div v-if="active == -1" class="block">
                <div class="block-title">
                    <div>
                        <!--服务器地址-->
                        {{$t("message.setUpPage.serverAddressTip")}}
                    </div>
                    <div>
                        <!--例: http://192.168.0.55:8181/-->
                        {{$t("message.setUpPage.serverAddressTip1")}}
                    </div>
                </div>
                <van-field v-model="serverAddress" :label="$t('message.setUpPage.serverAddress')" :placeholder="`${this.$t('pleaseInput')}${this.$t('message.setUpPage.serverAddress')}`"
                           :rules="[{ required: true, message: `${this.$t('pleaseInput')}${this.$t('message.setUpPage.serverAddress')}` }]">
                </van-field>
                <div style="margin: 16px;">
                    <!--测试地址中...-->
                    <van-button round block plain :type="serverAddressButtonType"
                                :loading="checkingServer" :loading-text="$t('message.setUpPage.serverAddressTesting')" @click="onSubmitServerAddress">
                        {{serverAddressButton}}
                    </van-button>
                </div>
            </div>
            <div v-if="active == 0" class="block">
                <div class="block">
                    <div class="block-title">
                        <!--权限-->
                        {{$t("message.setUpPage.permissions")}}
                    </div>
                    <van-cell-group>
                        <!-- 摄像头 已获取 : 未获取-->
                        <van-cell :title="$t('message.setUpPage.camera')" :label="hasPermission ? `${this.$t('message.setUpPage.obtained')}` : `${this.$t('message.setUpPage.notObtained')}`">
                            <template #right-icon>
                                <!--检测-->
                                <van-button size="small" @click="checkHasPersission">
                                    {{$t("message.setUpPage.check")}}
                                </van-button>
                            </template>
                        </van-cell>
                    </van-cell-group>
                </div>
                <van-radio-group v-model="selectedArSDKString">
                    <div class="block-title">
                        Google ARCore
                    </div>
                    <van-cell-group>
                        <!--使用 ARCore-->
                        <van-cell :title="$t('message.setUpPage.useARCore')" :label="arcoreStatusReason" :clickable="isArcoreReady" @click="isArcoreReady && selectARSDK(1)">
                            <template #right-icon>
                                <van-radio name="1" :disabled="!isArcoreReady" />
                            </template>
                        </van-cell>
                        <van-cell :title="arcoreSupportStatusText">
                            <template #right-icon>
                                <van-button size="small" @click="checkArcoreSupport">
                                    <!--检测-->
                                    {{$t('message.setUpPage.check')}}
                                </van-button>
                            </template>
                        </van-cell>
                        <van-cell :title="arcoreInstallStatusText">
                            <template #right-icon>
                                <van-button size="small" @click="checkArcoreInstall(true)">
                                    <!--安装-->
                                    {{$t('message.setUpPage.install')}}
                                </van-button>
                            </template>
                        </van-cell>
                    </van-cell-group>
                    <div class="block-title">
                        Huawei AREngine
                    </div>
                    <van-cell-group>
                        <van-cell title="使用 AREngine" :label="arengineStatusReason" :clickable="isArengineReady" @click="isArengineReady && selectARSDK(2)">
                            <template #right-icon>
                                <van-radio name="2" :disabled="!isArengineReady" />
                            </template>
                        </van-cell>
                        <van-cell :title="arengineSupportStatusText">
                            <template #right-icon>
                                <van-button size="small" @click="checkArengineSupport">
                                    <!-- 检测-->
                                    {{$t('message.setUpPage.check')}}
                                </van-button>
                            </template>
                        </van-cell>
                        <van-cell :title="arengineInstallStatusText">
                            <template #right-icon>
                                <!--安装-->
                                <van-button size="small" @click="checkArengineInstall(true)">
                                    {{$t('message.setUpPage.install')}}
                                </van-button>
                            </template>
                        </van-cell>
                    </van-cell-group>
                </van-radio-group>
                <div style="margin: 16px;">
                    <van-button round block type="primary" @click="onSubmit">
                        <!--确认-->
                        {{$t('submit')}}
                    </van-button>
                </div>
            </div>
        </section>
    </div>
</template>
<script>
    import { Toast, Dialog } from 'vant';
    import Fetch from '../utils/fetch';
    import store from '../store';
   /* import i18n from '@/utils/i18n/i18n'
    const { locale, t } = i18n.global*/

    export default {
        inject:['reload'],  //注入App里的reload方法
        data() {
            return {
                active: -1,
                checkingServer: false,
                serverAddress: '',
                currentLan: localStorage.getItem("lang") || "zh" ,
                dateTimeToFreshDom:"0", //因为语言切换没有立即生效，必须在“this.$nextTick”中更新 DOM 元素才能生效。这个元素隐藏在页面中，以便实现在切换语言时无需刷新页面即可自动生效。
                lanCategory: [
                    {text: '简体中文', value: "zh"},
                    {text: '繁體中文', value: 'tw'},
                    {text: 'English', value: 'en'}
                ],
            }
        },
        methods: {
            onSubmitServerAddress() {
                console.log('checking server', this.serverAddress, this.Host);
                /*请输服务器地址*/
                if (!this.serverAddress) {
                    Toast(`${this.$t('pleaseInput')}${this.$t('message.setUpPage.serverAddress')}`);
                    return;
                }
                try {
                    let url = new URL(this.serverAddress);
                    this.saveHost({host: this.serverAddress});
                    console.log("Setup serverAddr", url);
                    this.checkingServer = true;
                    Fetch.Get("/getVersionInfo")
                        .then((res) => {
                            console.log('检测服务器地址成功', res);
                            /*设置成功. 服务器版本：*/
                            Toast( this.$t('message.setUpPage.submitAddressResp0') + res.version);
                            this.active++;
                        })
                        .catch((e) => {
                            console.log('检测服务器地址失败', e);
                            /*检测服务器地址失败:*/
                            Toast(this.$t('message.setUpPage.submitAddressResp1') + JSON.stringify(e.message));
                        })
                        .finally(() => { this.checkingServer = false; });
                } catch (e) {
                    /*URL 格式不正确，正确格式：例: http://192.168.0.55:8181/*/
                    Toast(this.$t('message.setUpPage.submitAddressResp2'));
                }
            },
            onSubmit() {
                console.log('submit hasPermission', this.hasPermission);
                console.log('submit selectedArSDKType', this.selectedArSDKType);
                console.log('submit arcoreSupportStatus', this.arcoreSupportStatus);
                console.log('submit arcoreInstallStatus', this.arcoreInstallStatus);
                console.log('submit arengineSupportStatus', this.arengineSupportStatus);
                console.log('submit arengineInstallStatus', this.arengineInstallStatus);
                console.log('submit arengineInstallStatus', this.isARReady);

                this.updateStatus();

                if (!this.isARReady) {
                    /*没有找到可用的 AR SDK 环境，当前只支持SR/VR应用。*/
                    Dialog({ message: this.$t("message.setUpPage.unsupportedArTip") })
                        .then(() => {
                            this.active++;
                            this.$router.replace("/");
                        });
                } else {
                    /*"AR 环境已准备好进入首页"*/
                    Toast(this.$t("message.setUpPage.supportedArTip"));
                    this.active++;
                    this.$router.replace("/");
                }
            },
            async onChangeLan() {
                try {
                    await this.freshLan();
                } catch (e) {
                    console.warn(e);
                }
            }
            ,
            freshLan() {
                store.commit("freshLan", this.currentLan)
                this.$i18n.locale = this.currentLan;
                //因为语言切换没有立即生效，必须在“this.$nextTick”中更新 DOM 元素才能生效。
                this.$nextTick(() => {
                    this.dateTimeToFreshDom = new Date().getTime();
                   /* console.log(this.$i18n.locale)
                    console.log("this.message.setUpPage is :",this.$t())*/

                });
                this.reload();
                if(this.serverAddress != ""){
                    return new Promise((resolve, reject) => {
                        Fetch.Get("/heartbeat?language=" + this.currentLan).then(() => {
                            resolve();

                        }).catch(() => {
                            reject();
                        });
                    })
                }


            }

        },
        mounted() {
            if (!this.isArcoreReady && this.isArengineReady) {
                this.selectARSDK(2);
            }
            /*console.log(t)
            console.log(locale)*/
            // this.serverAddress = "http://222.128.6.137:8585/"
            // this.onSubmitServerAddress();
        },
        computed: {
            serverAddressButtonType() {
                switch (this.serverAddressStatus) {
                    case 0:
                        return 'primary';
                    case 1:
                        return 'success';
                    case 2:
                        return 'danger';
                    default:
                        return '';
                }
            },
            serverAddressButton() {
                switch (this.serverAddressStatus) {
                    case 0:
                        /*  '检测当前地址'*/
                        return this.$t("message.setUpPage.serverAddressStatus0");
                    case 1:
                        /* '当前地址成功'*/
                        return this.$t("message.setUpPage.serverAddressStatus1");
                    case 2:
                        /*'当前地址失败'*/
                        return this.$t("message.setUpPage.serverAddressStatus2");
                    default:
                        /* '检测当前地址'*/
                        return this.$t("message.setUpPage.serverAddressStatus0");
                }
            },
            serverAddressSrButtonType() {
                switch (this.serverAddressSrStatus) {
                    case 0:
                        return 'primary';
                    case 1:
                        return 'success';
                    case 2:
                        return 'danger';
                    default:
                        return '';
                }
            },
        },
    }
</script>
<style scoped>
    .section {
        box-sizing: border-box;
        min-height: calc(100vh - 56px);
        padding-bottom: 20px;
    }
    .section .block-title {
        margin: 0;
        padding: 32px 16px 16px;
        color: rgba(69,90,100,.6);
        font-weight: 400;
        font-size: 14px;
        line-height: 16px;
    }
</style>
