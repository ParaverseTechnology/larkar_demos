<template>
  <div class="setup">
    <van-nav-bar
        title="设置"
        left-text="返回"
        left-arrow
        @click-left="onClickLeft"
    />
     <section class="section">
        <div class="block">
            <div class="block-title">
                <div>
                    服务器地址
                </div>
                <div>
                    例: http://192.168.0.55:8181/
                </div>
            </div>
            <van-field v-model="serverAddress" label="地址" placeholder="请输服务器地址" 
                :rules="[{ required: true, message: '请输服务器地址' }]">
            </van-field>
            <div style="margin: 16px;">
                <van-button round block plain :type="serverAddressButtonType" :loading="checkingServer" loading-text="测试地址中..." @click="onSubmitServerAddress">
                    {{serverAddressButton}}
                </van-button>
            </div>
        </div>
        <div class="block">
            <van-radio-group v-model="selectedArSDKString">
                <div class="block-title">
                    Google ARCore
                </div>
                <van-cell-group>
                    <van-cell title="使用 ARCore" :label="arcoreStatusReason" :clickable="isArcoreReady" @click="isArcoreReady && selectARSDK(1)">
                        <template #right-icon>
                            <van-radio name="1" :disabled="!isArcoreReady" />
                        </template>
                    </van-cell>
                    <van-cell :title="arcoreSupportStatusText">
                        <template #right-icon>
                            <van-button size="small" @click="checkArcoreSupport">
                                检测
                            </van-button>
                        </template>
                    </van-cell>
                    <van-cell :title="arcoreInstallStatusText">
                        <template #right-icon>
                            <van-button size="small" @click="checkArcoreInstall(true)">
                                安装
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
                                检测
                            </van-button>
                        </template>
                    </van-cell>
                    <van-cell :title="arengineInstallStatusText">
                        <template #right-icon>
                            <van-button size="small" @click="checkArengineInstall(true)">
                                安装
                            </van-button>
                        </template>
                    </van-cell>
                </van-cell-group>
            </van-radio-group>
        </div>
    </section>
  </div>
</template>
<script>
import { Toast } from 'vant';
import Fetch from '../utils/fetch';

export default {
    data() {
        return {
            checkingServer: false,
            serverAddressStatus: 0,
            serverAddress: '',
        }
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
                    return '检测当前地址';
                case 1:
                    return '当前地址成功';
                case 2:
                    return '当前地址失败';
                default:
                    return '检测当前地址';
            }
        }
    },
    methods: {
        onClickLeft() {
            this.$router.back();
        },
        onSubmitServerAddress() {
            console.log('checking server', this.serverAddress, this.Host);
            if (!this.serverAddress) {
                Toast("请输服务器地址");
                return;
            }
            try {
                let url = new URL(this.serverAddress);
                this.saveHost(this.serverAddress);
                console.log("Setup serverAddr", url);
                this.checkingServer = true;
                Fetch.Get("/getVersionInfo")
                .then((res) => {
                    console.log('检测服务器地址成功', res);
                    Toast("设置成功: " + res.version);
                    this.serverAddressStatus = 1;
                })
                .catch((e) => {
                    console.log('检测服务器地址失败', e.message);
                    Toast("检测服务器地址失败:" + JSON.stringify(e.message));    
                    this.serverAddressStatus = 2;
                })
                .finally(() => { this.checkingServer = false; });
            } catch (e) {
                Toast("URL 格式不正确，正确格式：例: http://192.168.0.55:8181/");
            }
        },
    },
    mounted() {
        console.log('setup mounted');
        this.serverAddress = this.Host;
        this.serverAddressStatus = 0;
        console.log('setup mounted finished');
    },
    beforeUnmount() {
        console.log('setup beforeUnmount');
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