<template>
  <div class="container">
      <van-nav-bar
        title="LarkXR"
        right-text="设置"
        @click-right="onSetupPage"
      />
      <div class="banner">
        <img src="../assets/WX20210807-233343.jpg" alt="">
      </div>
      <van-dropdown-menu>
            <van-dropdown-item v-model="curretnAppliType" :options="category" @change="onFresh" />
            <van-dropdown-item v-model="region.regionId" :options="regionList" />
      </van-dropdown-menu>

        <van-pull-refresh 
            v-model="refreshing" 
            @refresh="onFresh">
            <van-list class="list"
                v-model:loading="loading"
                v-model:error="listError"
                :finished="finished"
                finished-text=""
                error-text="请求失败，点击重新"
                @load="onNextPage"
            >
                <van-grid :column-num="2" :gutter="10" :border="false" :center="false">
                    <van-grid-item 
                        class="appli-item" v-for="(item, index) in list" :key="index" @click="onEnterAppli" 
                        :data-appid="item.appliId" 
                        :data-appliType="item.appliType"
                        :data-instanceMax="item.instanceMax"
                    >
                        <div class="cover">
                            <div v-if="item.appliType == 9 || item.appliType == 11" class="appli-type">
                                <img src="../assets/icon-ar.png" alt="" />
                                <span v-if="item.appliType == 11">CloudXR</span>
                            </div>
                            <div v-else-if="item.appliType == 6 || item.appliType == 5 || item.appliType == 3" class="appli-type">
                                <img src="../assets/icon-vr.png" alt="" />
                                <span v-if="item.appliType == 6">CloudXR</span>
                            </div>
                            
                            <img v-if="item.picUrl" :src="item.picUrl" />
                            <img v-else src="../assets/default-cover.jpg" />
                            <div class="appli-status">
                                <span>{{item.runCnt}}/{{item.instanceMax}}</span>
                            </div>
                        </div>
                        <div class="appli-info van-multi-ellipsis--l2">
                            <div v-if="item.appliType == 13" class="appli-type">
                                <span v-if="item.appliType == 13">PixelStreaming</span>
                            </div>
                            <div>{{item.appliName}}</div>
                        </div>
                        <!-- <img src="../assets/arrow-right.png" class='icon-right' mode="aspectFill" /> -->
                    </van-grid-item>
                </van-grid>
            </van-list>
        </van-pull-refresh>
        <van-overlay :show="showLoading" @click="showLoading = false">
              <div class="loading-wrapper">
                <van-loading type="spinner" color="#1989fa" />
            </div>
        </van-overlay>
  </div>
</template>

<script>
import Fetch from '../utils/fetch'
import Native from '../utils/native'
import { Toast } from 'vant';

export default {
    name: 'Index',
    data() {
        return {
            curretnAppliType: "",
            category: [
                { text: '全部', value: "" },
                { text: 'AR', value: "AR" },
                { text: 'SR', value: 'SIM' },
                { text: 'VR', value: 'VR' },
            ],
            region: {
                delay: 9999,
                regionId: '',
                regionName: '',
            },
            regionList: [
                {
                    text: '未配置区域',
                    value: '',
                    regionId: '',
                }
            ],
            triggered: false,
            list: [],
            nextPage: 1,
            hasNextPage: false,
            pageNum: 1,
            showRegionList: false,
            refreshing: false,
            loading: false,
            listError: false,
            showLoading: false,
        }
    },
    computed: {
        finished() {
            return !this.hasNextPage;
        },
        regionClass() {
            return this.showRegionList ? "region-list show-region-list" : "region-list";
        },
    },
    methods: {
        async onFresh() {
            try {
                this.showLoading = true;
                await this.freshList();
            } catch (e) {
                console.warn(e);
                this.listError = true;
                Toast("请求失败，请确认网络连接和服务器地址是否正确");
            }
            this.showLoading = false;
            this.refreshing = false;
        },
        async onNextPage() {
            try {
                this.showLoading = true;
                await this.freshList(this.nextPage, false);
                if (!this.hasNextPage) {
                    Toast("没有更多啦~");
                }
            } catch (e) {
                console.warn(e);
                this.listError = true;
            }
            this.showLoading = false;
            this.loading = false;
            console.log("on next page");
        },
        freshList(page = 1, clearOld = true) {
            return new Promise((resolve, reject) => {
                Fetch.Get("getAppliList?appliType=" + this.curretnAppliType, {
                    pageSize: 12,
                    page,
                }).
                then((res) => {
                        console.log("fetch list success ", res);
                        let resPageInfo = {
                            current: parseInt(res.current),
                            pages: parseInt(res.pages)
                        }
                        let pageInfo = {
                            hasNextPage: resPageInfo.current < resPageInfo.pages,
                            nextPage: resPageInfo.current < resPageInfo.pages ? resPageInfo.current + 1 : resPageInfo.current,
                            pageNum: resPageInfo.current,
                        };
                        console.log("page info ", resPageInfo, pageInfo);
                        if (clearOld) {
                            this.hasNextPage = pageInfo.hasNextPage;
                            this.nextPage = pageInfo.nextPage;
                            this.pageNum = pageInfo.pageNum;
                            this.list = res.records;
                        } else {
                            this.hasNextPage = pageInfo.hasNextPage;
                            this.nextPage = pageInfo.nextPage;
                            this.pageNum = pageInfo.pageNum;
                            this.list = this.list.concat(res.records);
                        }
                        resolve();
                    })
                    .catch((e) => {
                        console.warn("fetch list failed ", e);
                        reject();
                    });
            })
        },
        freshRegionList() {
            Fetch.Get("renderServer/regionList")
            .then((res) => {
                if (res.length > 0) {
                    console.log('region list ', res, res.length);
                    this.regionList = res;
                    for (var i = 0; i < this.regionList.length; i++) {
                        this.regionList[i].text = this.regionList[i].regionName ? this.regionList[i].regionName : "本地服务器";
                        this.regionList[i].value = this.regionList[i].regionId;
                    }
                }
                this.region = this.regionList[0];
            })
            .catch((e) => {
                console.error(e);
            });
        },
        onEnterAppli(event) {
            // regionId: BJ
            const appliId = event.currentTarget.dataset.appid;
            const appliType = event.currentTarget.dataset.applitype;
            const instanceMax = event.currentTarget.dataset.instancemax;

            console.log("on enter appli", appliId, appliType, this.region, event.currentTarget.dataset, instanceMax);
            // AR type
            if (appliType == 9 || appliType == 11) {
                if (!Native.enableCloudXR() && appliType == 11) {
                    Toast("当前原生编译库未支持CloudXR.");
                    return;
                }
                if (!this.isARReady) {
                    Toast("AR环境未准备好，当前只支持SR应用");
                    return;
                }
                console.log("enter ar app");

                // Native.enterAppli(appliId, false);

                Fetch.Get("taskInfo/getRunningCnt", {appliId})
                .then((res) => {
                    console.log('getRunningCnt ', parseInt(res.total), instanceMax);
                    if (parseInt(res.total) <= instanceMax) {
                        Native.enterAppli(appliId, false);
                    } else {
                        Toast("渲染实例不足");
                    }
                })
                .catch((e) => {
                    console.warn('getRunningCnt failed', e);
                    Toast(JSON.stringify(e));
                })
            } else if (appliType == 2 || appliType == 1 || appliType == 13) { 
                // SR type
                console.log("enter ar app");
                Fetch.Get("appli/getStartInfo", {
                    appliId,
                    codeRate: 8000,
                    frameRate: 60,
                    regionId: this.region.regionId,
                })
                .then((res) => {
                    console.log("res ", res);
                    // this.$router.push('/WebClient?url=' + Fetch.ToWebClientUrl(res));
                    this.$router.push({ path: '/WebClient', query: { url: Fetch.ToWebClientUrl(res) } });
                })
                .catch((e) => {
                    console.log("on enter appli failed ", e);
                    Toast(JSON.stringify(e));
                });
            } else if (appliType == 3 || appliType == 5 || appliType == 6) {
                // VR type
                Fetch.Get("taskInfo/getRunningCnt", {appliId})
                .then((res) => {
                    console.log('getRunningCnt ', parseInt(res.total), instanceMax);
                    if (parseInt(res.total) <= instanceMax) {
                        Native.enterAppli(appliId, true);
                    } else {
                        Toast("渲染实例不足");
                    }
                })
                .catch((e) => {
                    console.warn('getRunningCnt failed', e);
                    Toast(JSON.stringify(e));
                })
            } else {
                console.log("ar app type not support");
                Toast("Unsupport appli type " + appliType);
            }
        },
        onToggleRegionList() {
            this.showRegionList = !this.showRegionList;
            console.log('on toggle region list', this.showRegionList, this.regionClass);
        },
        onChangeRegion(event) {
            const index = event.currentTarget.dataset.index;
            this.region = this.regionList[index];
            this.showRegionList = !this.showRegionList;
        },
        onSetupPage() {
            this.$router.push({ path: '/Setup' });
        },
    },
    mounted() {
        console.log('index mounted');
        console.log('index mounted Host', this.Host);
        console.log('index mounted hasPermission', this.hasPermission);
        console.log('index mounted isARReady', this.isARReady);
        if (!this.isARReady) {
            Toast("AR环境未准备好，当前只支持SR应用");
        }
        this.freshRegionList();
        this.onFresh();
        console.log('index mounted finished');
    },
    beforeUnmount() {
        console.log('beforeUnmount');
    },
}
</script>

<style>
.banner img {
    width: 100%;
}

.region {
    padding: 10px 10px;
}
.region-button {
    padding: 10px 0;
    border-bottom: 1px solid #d3d3d3;
}
.region-list {
    /* display: none; */
    /* height: 0; */
    transition: all 0.5s linear 0s;
    max-height: 0;
    overflow: hidden;
}
.selected-region {
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: space-between;
}
.selected-region image {
    width: 25px;
    height: 25px;
}
.region-toggle-button {
    display: flex;
    flex-direction: row;
    align-items: center;
}
.region-list.region-button {
    border: none;
    padding: 0;
}
.region-list.show-region-list {
    /* display:initial; */
    /* height: auto; */
    max-height: 120px;
}

.region-list.show-region-list.region-button {
    border-bottom: 1px solid #ddd;
    padding: 10px;
}

.list {
    padding: 10px;
}
.appli-item {
    display: flex;
}
.appli-item .van-grid-item__content {
    flex: 1;
    border-radius: 5px;
    padding: 0;
}
.appli-item .appli-info {
    padding: 0 8px;
    margin-bottom: 8px;
    font-size: 14px;
    color: #333;
    word-break: break-all;
}
.appli-item .cover {
    position: relative;
    margin-bottom: 8px;
}
.appli-item .cover img {
    border-top-left-radius: 5px;
    border-top-right-radius: 5px;
    vertical-align: middle; 
    width: 100%; 
    height: auto;
}
.appli-item .appli-type {
    position: absolute;
    top: 5px;
    left: 8px;
    color: #fff;
    font-size: 8px;
    display: flex;
    flex-direction: row;
    align-items: center;
    justify-content: center;
}
.appli-item .appli-status {
    position: absolute;
    bottom: 0px;
    color: #fff;
    width: 100%;
    font-size: 10px;
    padding: 5px 0;
    text-align: right;
}
.appli-item .appli-status span {
    margin-right: 5px;
}
.appli-item .appli-type img {
    width: 30px;
    height: auto;
    margin-right: 5px;
}
.appli-item .icon-right {
    width: 20px;
    height: 25px;
}
.loading-wrapper {
    display: flex;
    align-items: center;
    justify-content: center;
    height: 100%;
}
.loading-wrapper .block {
    width: 120px;
    height: 120px;
    background-color: #fff;
}
</style>