(function(e){function t(t){for(var r,o,a=t[0],s=t[1],u=t[2],l=0,b=[];l<a.length;l++)o=a[l],Object.prototype.hasOwnProperty.call(i,o)&&i[o]&&b.push(i[o][0]),i[o]=0;for(r in s)Object.prototype.hasOwnProperty.call(s,r)&&(e[r]=s[r]);d&&d(t);while(b.length)b.shift()();return c.push.apply(c,u||[]),n()}function n(){for(var e,t=0;t<c.length;t++){for(var n=c[t],r=!0,o=1;o<n.length;o++){var s=n[o];0!==i[s]&&(r=!1)}r&&(c.splice(t--,1),e=a(a.s=n[0]))}return e}var r={},i={app:0},c=[];function o(e){return a.p+"js/"+({about:"about"}[e]||e)+"."+{about:"9661689c"}[e]+".js"}function a(t){if(r[t])return r[t].exports;var n=r[t]={i:t,l:!1,exports:{}};return e[t].call(n.exports,n,n.exports,a),n.l=!0,n.exports}a.e=function(e){var t=[],n=i[e];if(0!==n)if(n)t.push(n[2]);else{var r=new Promise((function(t,r){n=i[e]=[t,r]}));t.push(n[2]=r);var c,s=document.createElement("script");s.charset="utf-8",s.timeout=120,a.nc&&s.setAttribute("nonce",a.nc),s.src=o(e);var u=new Error;c=function(t){s.onerror=s.onload=null,clearTimeout(l);var n=i[e];if(0!==n){if(n){var r=t&&("load"===t.type?"missing":t.type),c=t&&t.target&&t.target.src;u.message="Loading chunk "+e+" failed.\n("+r+": "+c+")",u.name="ChunkLoadError",u.type=r,u.request=c,n[1](u)}i[e]=void 0}};var l=setTimeout((function(){c({type:"timeout",target:s})}),12e4);s.onerror=s.onload=c,document.head.appendChild(s)}return Promise.all(t)},a.m=e,a.c=r,a.d=function(e,t,n){a.o(e,t)||Object.defineProperty(e,t,{enumerable:!0,get:n})},a.r=function(e){"undefined"!==typeof Symbol&&Symbol.toStringTag&&Object.defineProperty(e,Symbol.toStringTag,{value:"Module"}),Object.defineProperty(e,"__esModule",{value:!0})},a.t=function(e,t){if(1&t&&(e=a(e)),8&t)return e;if(4&t&&"object"===typeof e&&e&&e.__esModule)return e;var n=Object.create(null);if(a.r(n),Object.defineProperty(n,"default",{enumerable:!0,value:e}),2&t&&"string"!=typeof e)for(var r in e)a.d(n,r,function(t){return e[t]}.bind(null,r));return n},a.n=function(e){var t=e&&e.__esModule?function(){return e["default"]}:function(){return e};return a.d(t,"a",t),t},a.o=function(e,t){return Object.prototype.hasOwnProperty.call(e,t)},a.p="",a.oe=function(e){throw console.error(e),e};var s=window["webpackJsonp"]=window["webpackJsonp"]||[],u=s.push.bind(s);s.push=t,s=s.slice();for(var l=0;l<s.length;l++)t(s[l]);var d=u;c.push([0,"chunk-vendors"]),n()})({0:function(e,t,n){e.exports=n("56d7")},"56d7":function(e,t,n){"use strict";n.r(t);var r=n("5530"),i=(n("e260"),n("e6cf"),n("cca6"),n("a79d"),n("7a23"));function c(e,t,n,r,c,o){var a=Object(i["H"])("router-view");return Object(i["A"])(),Object(i["f"])(a)}n("ac1f"),n("5319");var o={mounted:function(){this.restoreHost(),this.restoreSDKType(),this.restoreQuickConfigLevel(),this.updateStatus(),console.log("global state",this.Host,this.isARReady,this.hasPermission,this.arcoreSupportStatus,this.selectedArSDKType),this.Host||this.$router.replace("/First")}},a=(n("a99d"),n("6b0d")),s=n.n(a);const u=s()(o,[["render",c]]);var l=u,d=(n("d3b7"),n("3ca3"),n("ddb0"),n("6c02")),b=n("679d"),p=n.n(b),g=n("ef31"),A=n.n(g),f=n("87d4"),v=n.n(f),h={class:"container"},S=Object(i["i"])("div",{class:"banner"},[Object(i["i"])("img",{src:p.a,alt:""})],-1),j={class:"cover"},O={key:0,class:"appli-type"},k=Object(i["i"])("img",{src:A.a,alt:""},null,-1),m=[k],R=["src"],I={key:2,src:v.a},y={class:"appli-status"},P={class:"appli-info van-multi-ellipsis--l2"},w={class:"loading-wrapper"};function C(e,t,n,r,c,o){var a=Object(i["H"])("van-nav-bar"),s=Object(i["H"])("van-dropdown-item"),u=Object(i["H"])("van-dropdown-menu"),l=Object(i["H"])("van-grid-item"),d=Object(i["H"])("van-grid"),b=Object(i["H"])("van-list"),p=Object(i["H"])("van-pull-refresh"),g=Object(i["H"])("van-loading"),A=Object(i["H"])("van-overlay");return Object(i["A"])(),Object(i["h"])("div",h,[Object(i["k"])(a,{title:"LarkXR","right-text":"设置",onClickRight:o.onSetupPage},null,8,["onClickRight"]),S,Object(i["k"])(u,null,{default:Object(i["P"])((function(){return[Object(i["k"])(s,{modelValue:c.curretnAppliType,"onUpdate:modelValue":t[0]||(t[0]=function(e){return c.curretnAppliType=e}),options:c.category,onChange:o.onFresh},null,8,["modelValue","options","onChange"]),Object(i["k"])(s,{modelValue:c.region.regionId,"onUpdate:modelValue":t[1]||(t[1]=function(e){return c.region.regionId=e}),options:c.regionList},null,8,["modelValue","options"])]})),_:1}),Object(i["k"])(p,{modelValue:c.refreshing,"onUpdate:modelValue":t[4]||(t[4]=function(e){return c.refreshing=e}),onRefresh:o.onFresh},{default:Object(i["P"])((function(){return[Object(i["k"])(b,{class:"list",loading:c.loading,"onUpdate:loading":t[2]||(t[2]=function(e){return c.loading=e}),error:c.listError,"onUpdate:error":t[3]||(t[3]=function(e){return c.listError=e}),finished:o.finished,"finished-text":"","error-text":"请求失败，点击重新",onLoad:o.onNextPage},{default:Object(i["P"])((function(){return[Object(i["k"])(d,{"column-num":2,gutter:10,border:!1,center:!1},{default:Object(i["P"])((function(){return[(Object(i["A"])(!0),Object(i["h"])(i["a"],null,Object(i["G"])(c.list,(function(e,t){return Object(i["A"])(),Object(i["f"])(l,{class:"appli-item",key:t,onClick:o.onEnterAppli,"data-appid":e.appliId,"data-appliType":e.appliType,"data-instanceMax":e.instanceMax},{default:Object(i["P"])((function(){return[Object(i["i"])("div",j,[9==e.appliType?(Object(i["A"])(),Object(i["h"])("div",O,m)):Object(i["g"])("",!0),e.picUrl?(Object(i["A"])(),Object(i["h"])("img",{key:1,src:e.picUrl},null,8,R)):(Object(i["A"])(),Object(i["h"])("img",I)),Object(i["i"])("div",y,[Object(i["i"])("span",null,Object(i["K"])(e.runCnt)+"/"+Object(i["K"])(e.limitMaxFps),1)])]),Object(i["i"])("div",P,[Object(i["i"])("div",null,Object(i["K"])(e.appliName),1)])]})),_:2},1032,["onClick","data-appid","data-appliType","data-instanceMax"])})),128))]})),_:1})]})),_:1},8,["loading","error","finished","onLoad"])]})),_:1},8,["modelValue","onRefresh"]),Object(i["k"])(A,{show:c.showLoading,onClick:t[5]||(t[5]=function(e){return c.showLoading=!1})},{default:Object(i["P"])((function(){return[Object(i["i"])("div",w,[Object(i["k"])(g,{type:"spinner",color:"#1989fa"})])]})),_:1},8,["show"])])}var L=n("1da1"),x=(n("99af"),n("96cf"),n("d4ec")),E=n("bee2"),T=n("bc3a"),B=n.n(T),H=(n("2b3d"),n("9861"),n("5502")),Q=n("ade3"),D=window["$Native"],M=function(){function e(){Object(x["a"])(this,e)}return Object(E["a"])(e,null,[{key:"showToast",value:function(e){null===D||void 0===D||D.showToast(e)}},{key:"enterAppli",value:function(e){null===D||void 0===D||D.enterAppli(e)}},{key:"onSaveServerAddress",value:function(e,t){console.log("native onSaveServerAddress ",e,parseInt(t)),null===D||void 0===D||D.onSaveServerAddress(e,parseInt(t))}},{key:"hasPermission",value:function(){return!!D&&(null===D||void 0===D?void 0:D.hasPermission())}},{key:"checkPermission",value:function(){return console.log("native checkPermission"),!!D&&(null===D||void 0===D?void 0:D.checkPermission())}},{key:"arcoreSupportStatus",value:function(){return console.log("native arcoreSupportStatus"),D?null===D||void 0===D?void 0:D.arcoreSupportStatus():-1}},{key:"arcoreInstallStatus",value:function(e){return console.log("native arcoreInstallStatus",e),D?null===D||void 0===D?void 0:D.arcoreInstallStatus(e):-1}},{key:"hwarengineSupportStatus",value:function(){return console.log("native hwarengineSupportStatus"),D?null===D||void 0===D?void 0:D.hwarengineSupportStatus():-1}},{key:"hwarengineInstallStatus",value:function(e){return console.log("native hwarengineInstallStatus",e),D?null===D||void 0===D?void 0:D.hwarengineInstallStatus(e):-1}},{key:"selectARSDK",value:function(e){return!!D&&(null===D||void 0===D?void 0:D.selectARSDK(e))}},{key:"selectQuickConfigLevel",value:function(e){return!!D&&(null===D||void 0===D?void 0:D.selectQuickConfigLevel(e))}}]),e}();Object(Q["a"])(M,"AR_SDK_TYPE_ARCORE",1),Object(Q["a"])(M,"AR_SDK_TYPE_HW_ARENGINE",2),Object(Q["a"])(M,"QuickConfigLevel_Fast",2),Object(Q["a"])(M,"QuickConfigLevel_Normal",3),Object(Q["a"])(M,"QuickConfigLevel_Extreme",4);var N=n("626a"),U=Object(H["a"])({state:{Host:"",hasPermission:!1,selectedArSDKType:M.AR_SDK_TYPE_ARCORE,arcoreSupportStatus:-1,arcoreInstallStatus:-1,arengineSupportStatus:-1,arengineInstallStatus:-1,quickConfigLevel:M.QuickConfigLevel_Fast},getters:{isArcoreReady:function(e){return(0==e.arcoreSupportStatus||1==e.arcoreSupportStatus)&&0==e.arcoreInstallStatus},isArengineReady:function(e){return(0==e.arengineSupportStatus||1==e.arengineSupportStatus)&&0==e.arengineInstallStatus},isARReady:function(e,t){return e.hasPermission&&(t.isArcoreReady||t.isArengineReady)},selectedArSDKString:function(e){return e.selectedArSDKType+""},arcoreSupportStatusText:function(e){var t=e.arcoreSupportStatus;switch(t){case 0:return"支持";case 1:return"支持但需要更新状态";case 2:return"不支持"}return"未知是否支持"},arcoreInstallStatusText:function(e){var t=e.arcoreInstallStatus;switch(t){case 0:return"已安装";case 1:return"需要安装";case 2:return"不支持"}return"未知是否安装"},arcoreStatusReason:function(e){return-1==e.arcoreSupportStatus?"未知环境":2==e.arcoreSupportStatus?"不支持":0==e.arcoreInstallStatus?1==e.arcoreSupportStatus?"已就绪,但需要更新状态":"已就绪":1==e.arcoreInstallStatus?"需要安装环境":"安装失败"},arengineSupportStatusText:function(e){var t=e.arengineSupportStatus;switch(t){case 0:return"支持";case 1:return"支持但需要更新状态";case 2:return"不支持"}return"未知是否支持"},arengineInstallStatusText:function(e){var t=e.arengineInstallStatus;switch(t){case 0:return"已安装";case 1:return"需要安装";case 2:return"不支持"}return"未知是否安装"},arengineStatusReason:function(e){return-1==e.arengineSupportStatus?"未知环境":2==e.arengineSupportStatus?"不支持":0==e.arengineInstallStatus?1==e.arengineSupportStatus?"已就绪,但需要更新状态":"已就绪":1==e.arengineInstallStatus?"需要安装环境":"安装失败"},quickConfigLevelString:function(e){return e.quickConfigLevel+""}},mutations:{setHost:function(e,t){e.Host=t},setSelectedArSDKType:function(e,t){e.selectedArSDKType=t,console.log("setSelectedArSDKType",t)},setHasPermission:function(e,t){e.hasPermission=t},setArcoreSupportStatus:function(e,t){e.arcoreSupportStatus=t},setArcoreInstallStatus:function(e,t){e.arcoreInstallStatus=t},setArengineSupportStatus:function(e,t){e.arengineSupportStatus=t},setArengineInstallStatus:function(e,t){e.arengineInstallStatus=t},setQuickConfigLevel:function(e,t){e.quickConfigLevel=t}},actions:{saveHost:function(e,t){var n=e.commit;n("setHost",t),localStorage.setItem("host",t);try{var r=new URL(t);console.log("native saveserver address ",r,r.hostname,r.port),M.onSaveServerAddress(r.hostname,r.port)}catch(i){console.warn("parse host url failed",t)}},restoreHost:function(e){var t=e.commit,n=localStorage.getItem("host");n&&t("setHost",n);try{var r=new URL(n);console.log("native saveserver address ",r,r.hostname,r.port),M.onSaveServerAddress(r.hostname,r.port)}catch(i){console.warn("parse host url failed",n)}},selectARSDK:function(e,t){var n=e.commit;n("setSelectedArSDKType",t),localStorage.setItem("sdkType",t),M.selectARSDK(t)},restoreSDKType:function(e){var t=e.commit,n=parseInt(localStorage.getItem("sdkType"));n?(t("setSelectedArSDKType",n),M.selectARSDK(n)):t("setSelectedArSDKType",M.AR_SDK_TYPE_ARCORE)},selectQuickConfigLevel:function(e,t){var n=e.commit;n("setQuickConfigLevel",t),localStorage.setItem("quickConfigLevel",t),M.selectQuickConfigLevel(parseInt(t))},restoreQuickConfigLevel:function(e){var t=e.commit,n=e.dispatch,r=localStorage.getItem("quickConfigLevel",r);if(!r)return n("selectQuickConfigLevel",M.QuickConfigLevel_Fast),void M.selectQuickConfigLevel(M.QuickConfigLevel_Fast);t("setQuickConfigLevel",parseInt(r)),M.selectQuickConfigLevel(parseInt(r))},updateStatus:function(e){var t=e.state,n=e.commit;n("setHasPermission",M.hasPermission()),n("setArcoreSupportStatus",M.arcoreSupportStatus()),0!=t.arcoreSupportStatus&&1!=t.arcoreSupportStatus||n("setArcoreInstallStatus",M.arcoreInstallStatus(!1)),n("setArengineSupportStatus",M.hwarengineSupportStatus()),0!=t.arengineSupportStatus&&1!=t.arengineSupportStatus||n("setArengineInstallStatus",M.hwarengineInstallStatus(!1))},checkHasPersission:function(e){var t=e.state,n=e.commit;n("setHasPermission",M.hasPermission()),t.hasPermission?Object(N["a"])("已获得权限"):Object(N["a"])("请授予权限以正常使用AR功能")},checkArcoreSupport:function(e){var t=e.commit,n=e.getters;t("setArcoreSupportStatus",M.arcoreSupportStatus()),Object(N["a"])(n.arcoreSupportStatusText)},checkArcoreInstall:function(e,t){var n=e.commit,r=e.getters;n("setArcoreInstallStatus",M.arcoreInstallStatus(t)),Object(N["a"])(r.arcoreInstallStatusText)},checkArengineSupport:function(e){var t=e.commit,n=e.getters;t("setArengineSupportStatus",M.hwarengineSupportStatus()),Object(N["a"])(n.arengineSupportStatusText)},checkArengineInstall:function(e,t){var n=e.commit,r=e.getters;n("setArengineInstallStatus",M.hwarengineInstallStatus(t)),Object(N["a"])(r.arengineInstallStatusText)}}}),_=1e3,V=function(){function e(){Object(x["a"])(this,e)}return Object(E["a"])(e,[{key:"Get",value:function(){var e=Object(L["a"])(regeneratorRuntime.mark((function e(t,n){var r;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return e.next=2,B.a.get(U.state.Host+"/"+t,{params:n});case 2:if(r=e.sent,!r.data){e.next=11;break}if(r.data.code!=_){e.next=8;break}return e.abrupt("return",r.data.result);case 8:throw r.data.message;case 9:e.next=12;break;case 11:throw r;case 12:case"end":return e.stop()}}),e)})));function t(t,n){return e.apply(this,arguments)}return t}()},{key:"asyncPost",value:function(){var e=Object(L["a"])(regeneratorRuntime.mark((function e(t,n){var r,i;return regeneratorRuntime.wrap((function(e){while(1)switch(e.prev=e.next){case 0:return r=U.state.Host+"/"+t,e.next=3,B.a.post(r,n);case 3:if(i=e.sent,!i.data){e.next=12;break}if(i.data.code!=_){e.next=9;break}return e.abrupt("return",i.data.result);case 9:throw i.data.message;case 10:e.next=13;break;case 12:throw i;case 13:case"end":return e.stop()}}),e)})));function t(t,n){return e.apply(this,arguments)}return t}()},{key:"ToWebClientUrl",value:function(e){return encodeURIComponent(U.state.Host+"/webclient?"+K(e))}}]),e}();function K(e){var t="";for(var n in e)n&&(t+=n+"="+e[n]+"&");return t}var F=new V,X=F,z={name:"Index",data:function(){return{curretnAppliType:"",category:[{text:"全部",value:""},{text:"PXYAR",value:9},{text:"SR",value:"SIM"}],region:{delay:9999,regionId:"",regionName:""},regionList:[{text:"未配置区域",value:"",regionId:""}],triggered:!1,list:[],nextPage:1,hasNextPage:!1,pageNum:1,showRegionList:!1,refreshing:!1,loading:!1,listError:!1,showLoading:!1}},computed:{finished:function(){return!this.hasNextPage},regionClass:function(){return this.showRegionList?"region-list show-region-list":"region-list"}},methods:{onFresh:function(){var e=this;return Object(L["a"])(regeneratorRuntime.mark((function t(){return regeneratorRuntime.wrap((function(t){while(1)switch(t.prev=t.next){case 0:return t.prev=0,e.showLoading=!0,t.next=4,e.freshList();case 4:t.next=11;break;case 6:t.prev=6,t.t0=t["catch"](0),console.warn(t.t0),e.listError=!0,Object(N["a"])("请求失败，请确认网络连接和服务器地址是否正确");case 11:e.showLoading=!1,e.refreshing=!1;case 13:case"end":return t.stop()}}),t,null,[[0,6]])})))()},onNextPage:function(){var e=this;return Object(L["a"])(regeneratorRuntime.mark((function t(){return regeneratorRuntime.wrap((function(t){while(1)switch(t.prev=t.next){case 0:return t.prev=0,e.showLoading=!0,t.next=4,e.freshList(e.nextPage,!1);case 4:e.hasNextPage||Object(N["a"])("没有更多啦~"),t.next=11;break;case 7:t.prev=7,t.t0=t["catch"](0),console.warn(t.t0),e.listError=!0;case 11:e.showLoading=!1,e.loading=!1,console.log("on next page");case 14:case"end":return t.stop()}}),t,null,[[0,7]])})))()},freshList:function(){var e=this,t=arguments.length>0&&void 0!==arguments[0]?arguments[0]:1,n=!(arguments.length>1&&void 0!==arguments[1])||arguments[1];return new Promise((function(r,i){X.Get("getAppliList?appliType="+e.curretnAppliType,{pageSize:12,page:t}).then((function(t){console.log("fetch list success ",t);var i={current:parseInt(t.current),pages:parseInt(t.pages)},c={hasNextPage:i.current<i.pages,nextPage:i.current<i.pages?i.current+1:i.current,pageNum:i.current};console.log("page info ",i,c),n?(e.hasNextPage=c.hasNextPage,e.nextPage=c.nextPage,e.pageNum=c.pageNum,e.list=t.records):(e.hasNextPage=c.hasNextPage,e.nextPage=c.nextPage,e.pageNum=c.pageNum,e.list=e.list.concat(t.records)),r()})).catch((function(e){console.warn("fetch list failed ",e),i()}))}))},freshRegionList:function(){var e=this;X.Get("renderServer/regionList").then((function(t){if(t.length>0){console.log("region list ",t,t.length),e.regionList=t;for(var n=0;n<e.regionList.length;n++)e.regionList[n].text=e.regionList[n].regionName?e.regionList[n].regionName:"本地服务器",e.regionList[n].value=e.regionList[n].regionId}e.region=e.regionList[0]})).catch((function(e){console.error(e)}))},onEnterAppli:function(e){var t=this,n=e.currentTarget.dataset.appid,r=e.currentTarget.dataset.applitype,i=e.currentTarget.dataset.instancemax;if(console.log("on enter appli",n,r,this.region,e.currentTarget.dataset,i),9==r){if(!this.isARReady)return void Object(N["a"])("AR环境未准备好，当前只支持SR应用");console.log("enter ar app"),X.Get("/taskInfo/getRunningCnt",{appliId:n}).then((function(e){console.log("getRunningCnt ",parseInt(e.total),i),parseInt(e.total)<=i?M.enterAppli(n):M.showToast("渲染实例不足")})).catch((function(e){console.warn("getRunningCnt failed",e)}))}else 2==r||1==r||13==r?(console.log("enter ar app"),X.Get("appli/getStartInfo",{appliId:n,codeRate:8e3,frameRate:60,regionId:this.region.regionId}).then((function(e){console.log("res ",e),t.$router.push({path:"/WebClient",query:{url:X.ToWebClientUrl(e)}})})).catch((function(e){console.log("on enter appli failed ",e)}))):(console.log("ar app type not support"),Object(N["a"])("Unsupport appli type "+r))},onToggleRegionList:function(){this.showRegionList=!this.showRegionList,console.log("on toggle region list",this.showRegionList,this.regionClass)},onChangeRegion:function(e){var t=e.currentTarget.dataset.index;this.region=this.regionList[t],this.showRegionList=!this.showRegionList},onSetupPage:function(){this.$router.push({path:"/Setup"})}},mounted:function(){console.log("index mounted"),console.log("index mounted Host",this.Host),console.log("index mounted hasPermission",this.hasPermission),console.log("index mounted isARReady",this.isARReady),this.isARReady||Object(N["a"])("AR环境未准备好，当前只支持SR应用"),this.freshRegionList(),this.onFresh(),console.log("index mounted finished")},beforeUnmount:function(){console.log("beforeUnmount")}};n("c043");const J=s()(z,[["render",C]]);var G=J,Y={class:"container"},Z=["src"];function W(e,t,n,r,c,o){var a=Object(i["H"])("van-nav-bar");return Object(i["A"])(),Object(i["h"])("div",Y,[Object(i["k"])(a,{title:"LarkXR","left-text":"返回","left-arrow":"",onClickLeft:o.onClickLeft},null,8,["onClickLeft"]),Object(i["i"])("iframe",{frameborder:"0",id:"iframe",src:c.url},"\r\n        ",8,Z)])}var q={data:function(){return{url:""}},methods:{onClickLeft:function(){this.$router.back()},onMessage:function(e){920==e.data.type&&this.$router.back()}},mounted:function(){this.url=window.decodeURIComponent(this.$route.query.url),console.log("load iframe ",this.$route.query.url,this.url),window.addEventListener("message",this.onMessage,!1)},beforeMount:function(){window.removeEventListener("message",this.onMessage,!1),this.url=""}};n("7506");const $=s()(q,[["render",W],["__scopeId","data-v-54c7be3e"]]);var ee=$,te=function(e){return Object(i["D"])("data-v-80a864d0"),e=e(),Object(i["B"])(),e},ne={class:"setup"},re={class:"section"},ie={class:"block"},ce=te((function(){return Object(i["i"])("div",{class:"block-title"},[Object(i["i"])("div",null," 服务器地址 "),Object(i["i"])("div",null," 例: http://192.168.0.55:8181/ ")],-1)})),oe={style:{margin:"16px"}},ae={class:"block"},se=te((function(){return Object(i["i"])("div",{class:"block-title"}," 权限 ",-1)})),ue=Object(i["j"])(" 检测 "),le={class:"block"},de=te((function(){return Object(i["i"])("div",{class:"block-title"}," Google ARCore ",-1)})),be=Object(i["j"])(" 检测 "),pe=Object(i["j"])(" 安装 "),ge=te((function(){return Object(i["i"])("div",{class:"block-title"}," Huawei AREngine ",-1)})),Ae=Object(i["j"])(" 检测 "),fe=Object(i["j"])(" 安装 "),ve={class:"block"},he=te((function(){return Object(i["i"])("div",{class:"block-title"}," 快速设置 ",-1)}));function Se(e,t,n,r,c,o){var a=Object(i["H"])("van-nav-bar"),s=Object(i["H"])("van-field"),u=Object(i["H"])("van-button"),l=Object(i["H"])("van-tab"),d=Object(i["H"])("van-cell"),b=Object(i["H"])("van-cell-group"),p=Object(i["H"])("van-radio"),g=Object(i["H"])("van-radio-group"),A=Object(i["H"])("van-tabs");return Object(i["A"])(),Object(i["h"])("div",ne,[Object(i["k"])(a,{title:"设置","left-text":"返回","left-arrow":"",onClickLeft:o.onClickLeft},null,8,["onClickLeft"]),Object(i["i"])("section",re,[Object(i["k"])(A,{active:c.active,"onUpdate:active":t[10]||(t[10]=function(e){return c.active=e}),sticky:"",animated:""},{default:Object(i["P"])((function(){return[Object(i["k"])(l,{title:"服务器地址"},{default:Object(i["P"])((function(){return[Object(i["i"])("div",ie,[ce,Object(i["k"])(s,{modelValue:c.serverAddress,"onUpdate:modelValue":t[0]||(t[0]=function(e){return c.serverAddress=e}),label:"地址",placeholder:"请输服务器地址",rules:[{required:!0,message:"请输服务器地址"}]},null,8,["modelValue"]),Object(i["i"])("div",oe,[Object(i["k"])(u,{round:"",block:"",plain:"",type:o.serverAddressButtonType,loading:c.checkingServer,"loading-text":"测试地址中...",onClick:o.onSubmitServerAddress},{default:Object(i["P"])((function(){return[Object(i["j"])(Object(i["K"])(o.serverAddressButton),1)]})),_:1},8,["type","loading","onClick"])])])]})),_:1}),Object(i["k"])(l,{title:"AR 环境"},{default:Object(i["P"])((function(){return[Object(i["i"])("div",ae,[se,Object(i["k"])(b,null,{default:Object(i["P"])((function(){return[Object(i["k"])(d,{title:"摄像头",label:e.hasPermission?"已获取":"未获取"},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(u,{size:"small",onClick:e.checkHasPersission},{default:Object(i["P"])((function(){return[ue]})),_:1},8,["onClick"])]})),_:1},8,["label"])]})),_:1})]),Object(i["i"])("div",le,[Object(i["k"])(g,{modelValue:e.selectedArSDKString,"onUpdate:modelValue":t[5]||(t[5]=function(t){return e.selectedArSDKString=t})},{default:Object(i["P"])((function(){return[de,Object(i["k"])(b,null,{default:Object(i["P"])((function(){return[Object(i["k"])(d,{title:"使用 ARCore",label:e.arcoreStatusReason,clickable:e.isArcoreReady,onClick:t[1]||(t[1]=function(t){return e.isArcoreReady&&e.selectARSDK(1)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(p,{name:"1",disabled:!e.isArcoreReady},null,8,["disabled"])]})),_:1},8,["label","clickable"]),Object(i["k"])(d,{title:e.arcoreSupportStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(u,{size:"small",onClick:e.checkArcoreSupport},{default:Object(i["P"])((function(){return[be]})),_:1},8,["onClick"])]})),_:1},8,["title"]),Object(i["k"])(d,{title:e.arcoreInstallStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(u,{size:"small",onClick:t[2]||(t[2]=function(t){return e.checkArcoreInstall(!0)})},{default:Object(i["P"])((function(){return[pe]})),_:1})]})),_:1},8,["title"])]})),_:1}),ge,Object(i["k"])(b,null,{default:Object(i["P"])((function(){return[Object(i["k"])(d,{title:"使用 AREngine",label:e.arengineStatusReason,clickable:e.isArengineReady,onClick:t[3]||(t[3]=function(t){return e.isArengineReady&&e.selectARSDK(2)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(p,{name:"2",disabled:!e.isArengineReady},null,8,["disabled"])]})),_:1},8,["label","clickable"]),Object(i["k"])(d,{title:e.arengineSupportStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(u,{size:"small",onClick:e.checkArengineSupport},{default:Object(i["P"])((function(){return[Ae]})),_:1},8,["onClick"])]})),_:1},8,["title"]),Object(i["k"])(d,{title:e.arengineInstallStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(u,{size:"small",onClick:t[4]||(t[4]=function(t){return e.checkArengineInstall(!0)})},{default:Object(i["P"])((function(){return[fe]})),_:1})]})),_:1},8,["title"])]})),_:1})]})),_:1},8,["modelValue"])])]})),_:1}),Object(i["k"])(l,{title:"LarkXR"},{default:Object(i["P"])((function(){return[Object(i["i"])("div",ve,[he,Object(i["k"])(g,{modelValue:e.quickConfigLevelString,"onUpdate:modelValue":t[9]||(t[9]=function(t){return e.quickConfigLevelString=t})},{default:Object(i["P"])((function(){return[Object(i["k"])(b,null,{default:Object(i["P"])((function(){return[Object(i["k"])(d,{title:"流畅",clickable:"",onClick:t[6]||(t[6]=function(t){return e.selectQuickConfigLevel(2)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(p,{name:"2"})]})),_:1}),Object(i["k"])(d,{title:"标准",clickable:"",onClick:t[7]||(t[7]=function(t){return e.selectQuickConfigLevel(3)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(p,{name:"3"})]})),_:1}),Object(i["k"])(d,{title:"高清",clickable:"",onClick:t[8]||(t[8]=function(t){return e.selectQuickConfigLevel(4)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(p,{name:"4"})]})),_:1})]})),_:1})]})),_:1},8,["modelValue"])])]})),_:1})]})),_:1},8,["active"])])])}n("e9c4");var je={data:function(){return{active:0,checkingServer:!1,serverAddressStatus:0,serverAddress:""}},computed:{serverAddressButtonType:function(){switch(this.serverAddressStatus){case 0:return"primary";case 1:return"success";case 2:return"danger";default:return""}},serverAddressButton:function(){switch(this.serverAddressStatus){case 0:return"检测当前地址";case 1:return"当前地址成功";case 2:return"当前地址失败";default:return"检测当前地址"}}},methods:{onClickLeft:function(){this.$router.back()},onSubmitServerAddress:function(){var e=this;if(console.log("checking server",this.serverAddress,this.Host),this.serverAddress)try{var t=new URL(this.serverAddress);this.saveHost(this.serverAddress),console.log("Setup serverAddr",t),this.checkingServer=!0,X.Get("/getVersionInfo").then((function(t){console.log("检测服务器地址成功",t),Object(N["a"])("设置成功. 服务器版本："+t.version),e.serverAddressStatus=1})).catch((function(t){console.log("检测服务器地址失败",t.message),Object(N["a"])("检测服务器地址失败:"+JSON.stringify(t.message)),e.serverAddressStatus=2})).finally((function(){e.checkingServer=!1}))}catch(n){Object(N["a"])("URL 格式不正确，正确格式：例: http://192.168.0.55:8181/")}else Object(N["a"])("请输服务器地址")}},mounted:function(){console.log("setup mounted"),this.serverAddress=this.Host,this.serverAddressStatus=0,console.log("setup mounted finished")},beforeUnmount:function(){console.log("setup beforeUnmount")}};n("c8ef");const Oe=s()(je,[["render",Se],["__scopeId","data-v-80a864d0"]]);var ke=Oe,me=function(e){return Object(i["D"])("data-v-467252f8"),e=e(),Object(i["B"])(),e},Re={class:"setup"},Ie={class:"section"},ye={class:"block"},Pe=me((function(){return Object(i["i"])("div",{class:"block-title"}," 进度 ",-1)})),we=Object(i["j"])("设置服务器地址"),Ce=Object(i["j"])("检查AR SDK"),Le=Object(i["j"])("成功"),xe={key:0,class:"block"},Ee=me((function(){return Object(i["i"])("div",{class:"block-title"},[Object(i["i"])("div",null," 服务器地址 "),Object(i["i"])("div",null," 例: http://192.168.0.55:8181/ ")],-1)})),Te={style:{margin:"16px"}},Be=Object(i["j"])(" 确认当前地址 "),He={key:1,class:"block"},Qe={class:"block"},De=me((function(){return Object(i["i"])("div",{class:"block-title"}," 权限 ",-1)})),Me=Object(i["j"])(" 检测 "),Ne=me((function(){return Object(i["i"])("div",{class:"block-title"}," Google ARCore ",-1)})),Ue=Object(i["j"])(" 检测 "),_e=Object(i["j"])(" 安装 "),Ve=me((function(){return Object(i["i"])("div",{class:"block-title"}," Huawei AREngine ",-1)})),Ke=Object(i["j"])(" 检测 "),Fe=Object(i["j"])(" 安装 "),Xe={style:{margin:"16px"}},ze=Object(i["j"])(" 确认 ");function Je(e,t,n,r,c,o){var a=Object(i["H"])("van-nav-bar"),s=Object(i["H"])("van-step"),u=Object(i["H"])("van-steps"),l=Object(i["H"])("van-field"),d=Object(i["H"])("van-button"),b=Object(i["H"])("van-cell"),p=Object(i["H"])("van-cell-group"),g=Object(i["H"])("van-radio"),A=Object(i["H"])("van-radio-group");return Object(i["A"])(),Object(i["h"])("div",Re,[Object(i["k"])(a,{title:"初始设置"}),Object(i["i"])("section",Ie,[Object(i["i"])("div",ye,[Pe,Object(i["k"])(u,{active:c.active},{default:Object(i["P"])((function(){return[Object(i["k"])(s,null,{default:Object(i["P"])((function(){return[we]})),_:1}),Object(i["k"])(s,null,{default:Object(i["P"])((function(){return[Ce]})),_:1}),Object(i["k"])(s,null,{default:Object(i["P"])((function(){return[Le]})),_:1})]})),_:1},8,["active"])]),-1==c.active?(Object(i["A"])(),Object(i["h"])("div",xe,[Ee,Object(i["k"])(l,{modelValue:c.serverAddress,"onUpdate:modelValue":t[0]||(t[0]=function(e){return c.serverAddress=e}),label:"地址",placeholder:"请输服务器地址",rules:[{required:!0,message:"请输服务器地址"}]},null,8,["modelValue"]),Object(i["i"])("div",Te,[Object(i["k"])(d,{round:"",block:"",type:"primary",loading:c.checkingServer,"loading-text":"测试地址中...",onClick:o.onSubmitServerAddress},{default:Object(i["P"])((function(){return[Be]})),_:1},8,["loading","onClick"])])])):Object(i["g"])("",!0),0==c.active?(Object(i["A"])(),Object(i["h"])("div",He,[Object(i["i"])("div",Qe,[De,Object(i["k"])(p,null,{default:Object(i["P"])((function(){return[Object(i["k"])(b,{title:"摄像头",label:e.hasPermission?"已获取":"未获取"},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(d,{size:"small",onClick:e.checkHasPersission},{default:Object(i["P"])((function(){return[Me]})),_:1},8,["onClick"])]})),_:1},8,["label"])]})),_:1})]),Object(i["k"])(A,{modelValue:e.selectedArSDKString,"onUpdate:modelValue":t[5]||(t[5]=function(t){return e.selectedArSDKString=t})},{default:Object(i["P"])((function(){return[Ne,Object(i["k"])(p,null,{default:Object(i["P"])((function(){return[Object(i["k"])(b,{title:"使用 ARCore",label:e.arcoreStatusReason,clickable:e.isArcoreReady,onClick:t[1]||(t[1]=function(t){return e.isArcoreReady&&e.selectARSDK(1)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(g,{name:"1",disabled:!e.isArcoreReady},null,8,["disabled"])]})),_:1},8,["label","clickable"]),Object(i["k"])(b,{title:e.arcoreSupportStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(d,{size:"small",onClick:e.checkArcoreSupport},{default:Object(i["P"])((function(){return[Ue]})),_:1},8,["onClick"])]})),_:1},8,["title"]),Object(i["k"])(b,{title:e.arcoreInstallStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(d,{size:"small",onClick:t[2]||(t[2]=function(t){return e.checkArcoreInstall(!0)})},{default:Object(i["P"])((function(){return[_e]})),_:1})]})),_:1},8,["title"])]})),_:1}),Ve,Object(i["k"])(p,null,{default:Object(i["P"])((function(){return[Object(i["k"])(b,{title:"使用 AREngine",label:e.arengineStatusReason,clickable:e.isArengineReady,onClick:t[3]||(t[3]=function(t){return e.isArengineReady&&e.selectARSDK(2)})},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(g,{name:"2",disabled:!e.isArengineReady},null,8,["disabled"])]})),_:1},8,["label","clickable"]),Object(i["k"])(b,{title:e.arengineSupportStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(d,{size:"small",onClick:e.checkArengineSupport},{default:Object(i["P"])((function(){return[Ke]})),_:1},8,["onClick"])]})),_:1},8,["title"]),Object(i["k"])(b,{title:e.arengineInstallStatusText},{"right-icon":Object(i["P"])((function(){return[Object(i["k"])(d,{size:"small",onClick:t[4]||(t[4]=function(t){return e.checkArengineInstall(!0)})},{default:Object(i["P"])((function(){return[Fe]})),_:1})]})),_:1},8,["title"])]})),_:1})]})),_:1},8,["modelValue"]),Object(i["i"])("div",Xe,[Object(i["k"])(d,{round:"",block:"",type:"primary",onClick:o.onSubmit},{default:Object(i["P"])((function(){return[ze]})),_:1},8,["onClick"])])])):Object(i["g"])("",!0)])])}var Ge=n("75e6"),Ye={data:function(){return{active:-1,checkingServer:!1,serverAddress:""}},methods:{onSubmitServerAddress:function(){var e=this;if(console.log("checking server",this.serverAddress,this.Host),this.serverAddress)try{var t=new URL(this.serverAddress);this.saveHost(this.serverAddress),console.log("Setup serverAddr",t),this.checkingServer=!0,X.Get("/getVersionInfo").then((function(t){console.log("检测服务器地址成功",t),Object(N["a"])("设置成功: "+t.version),e.active++})).catch((function(e){console.log("检测服务器地址失败",e),Object(N["a"])("检测服务器地址失败:"+JSON.stringify(e))})).finally((function(){e.checkingServer=!1}))}catch(n){Object(N["a"])("URL 格式不正确，正确格式：例: http://192.168.0.55:8181/")}else Object(N["a"])("请输服务器地址")},onSubmit:function(){var e=this;console.log("submit hasPermission",this.hasPermission),console.log("submit selectedArSDKType",this.selectedArSDKType),console.log("submit arcoreSupportStatus",this.arcoreSupportStatus),console.log("submit arcoreInstallStatus",this.arcoreInstallStatus),console.log("submit arengineSupportStatus",this.arengineSupportStatus),console.log("submit arengineInstallStatus",this.arengineInstallStatus),console.log("submit arengineInstallStatus",this.isARReady),this.updateStatus(),this.isARReady?(Object(N["a"])("AR 环境已准备好进入首页"),this.active++,this.$router.replace("/")):Object(Ge["a"])({message:"没有找到可用的 AR SDK 环境，当前只支持SR应用。"}).then((function(){e.active++,e.$router.replace("/")}))}},mounted:function(){!this.isArcoreReady&&this.isArengineReady&&this.selectARSDK(2)}};n("7cd3");const Ze=s()(Ye,[["render",Je],["__scopeId","data-v-467252f8"]]);var We=Ze,qe=[{path:"/",name:"Index",component:G},{path:"/WebClient",name:"WebClient",component:ee},{path:"/Setup",name:"Setup",component:ke},{path:"/First",name:"First",component:We},{path:"/about",name:"About",component:function(){return n.e("about").then(n.bind(null,"f820"))}}],$e=Object(d["a"])({history:Object(d["b"])(),routes:qe}),et=$e,tt=n("b970"),nt=(n("157a"),{computed:Object(r["a"])(Object(r["a"])({},Object(H["e"])({Host:function(e){return e.Host},selectedArSDKType:function(e){return e.selectedArSDKType},hasPermission:function(e){return e.hasPermission},arcoreSupportStatus:function(e){return e.arcoreSupportStatus},arcoreInstallStatus:function(e){return e.arcoreInstallStatus},arengineSupportStatus:function(e){return e.arengineSupportStatus},arengineInstallStatus:function(e){return e.arengineInstallStatus},quickConfigLevel:function(e){return e.quickConfigLevel}})),Object(H["c"])({isArcoreReady:"isArcoreReady",isArengineReady:"isArengineReady",isARReady:"isARReady",arcoreStatusReason:"arcoreStatusReason",arengineStatusReason:"arengineStatusReason",arcoreSupportStatusText:"arcoreSupportStatusText",arcoreInstallStatusText:"arcoreInstallStatusText",arengineSupportStatusText:"arengineSupportStatusText",arengineInstallStatusText:"arengineInstallStatusText",selectedArSDKString:"selectedArSDKString",quickConfigLevelString:"quickConfigLevelString"})),methods:Object(r["a"])(Object(r["a"])({},Object(H["d"])({})),Object(H["b"])({saveHost:"saveHost",restoreHost:"restoreHost",updateStatus:"updateStatus",checkHasPersission:"checkHasPersission",checkArcoreSupport:"checkArcoreSupport",checkArcoreInstall:"checkArcoreInstall",checkArengineSupport:"checkArengineSupport",checkArengineInstall:"checkArengineInstall",selectARSDK:"selectARSDK",restoreSDKType:"restoreSDKType",selectQuickConfigLevel:"selectQuickConfigLevel",restoreQuickConfigLevel:"restoreQuickConfigLevel"}))});Object(i["e"])(l).use(U).use(et).use(tt["a"]).mixin(nt).mount("#app")},"679d":function(e,t,n){e.exports=n.p+"img/WX20210807-233343.b5473b8f.jpg"},7506:function(e,t,n){"use strict";n("8473")},"7cd3":function(e,t,n){"use strict";n("a7f2")},8473:function(e,t,n){},"87d4":function(e,t){e.exports="data:image/jpeg;base64,/9j/4QAYRXhpZgAASUkqAAgAAAAAAAAAAAAAAP/sABFEdWNreQABAAQAAAAeAAD/4QMvaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wLwA8P3hwYWNrZXQgYmVnaW49Iu+7vyIgaWQ9Ilc1TTBNcENlaGlIenJlU3pOVGN6a2M5ZCI/PiA8eDp4bXBtZXRhIHhtbG5zOng9ImFkb2JlOm5zOm1ldGEvIiB4OnhtcHRrPSJBZG9iZSBYTVAgQ29yZSA1LjYtYzE0MiA3OS4xNjA5MjQsIDIwMTcvMDcvMTMtMDE6MDY6MzkgICAgICAgICI+IDxyZGY6UkRGIHhtbG5zOnJkZj0iaHR0cDovL3d3dy53My5vcmcvMTk5OS8wMi8yMi1yZGYtc3ludGF4LW5zIyI+IDxyZGY6RGVzY3JpcHRpb24gcmRmOmFib3V0PSIiIHhtbG5zOnhtcE1NPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvbW0vIiB4bWxuczpzdFJlZj0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL3NUeXBlL1Jlc291cmNlUmVmIyIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIiB4bXBNTTpEb2N1bWVudElEPSJ4bXAuZGlkOkM5QjQzNjMyNDEyNzExRUM4QTE0OEM1OThGOEVCQjBEIiB4bXBNTTpJbnN0YW5jZUlEPSJ4bXAuaWlkOkM5QjQzNjMxNDEyNzExRUM4QTE0OEM1OThGOEVCQjBEIiB4bXA6Q3JlYXRvclRvb2w9IkFkb2JlIFBob3Rvc2hvcCBDQyAyMDE3IChXaW5kb3dzKSI+IDx4bXBNTTpEZXJpdmVkRnJvbSBzdFJlZjppbnN0YW5jZUlEPSJ4bXAuaWlkOkJDRjE0RENBMUE5NTExRUJCMzE2RUQzNDFFRkVENTkxIiBzdFJlZjpkb2N1bWVudElEPSJ4bXAuZGlkOkJDRjE0RENCMUE5NTExRUJCMzE2RUQzNDFFRkVENTkxIi8+IDwvcmRmOkRlc2NyaXB0aW9uPiA8L3JkZjpSREY+IDwveDp4bXBtZXRhPiA8P3hwYWNrZXQgZW5kPSJyIj8+/+4ADkFkb2JlAGTAAAAAAf/bAIQAEAsLCwwLEAwMEBcPDQ8XGxQQEBQbHxcXFxcXHx4XGhoaGhceHiMlJyUjHi8vMzMvL0BAQEBAQEBAQEBAQEBAQAERDw8RExEVEhIVFBEUERQaFBYWFBomGhocGhomMCMeHh4eIzArLicnJy4rNTUwMDU1QEA/QEBAQEBAQEBAQEBA/8AAEQgAVACWAwEiAAIRAQMRAf/EAHMAAQEBAQEBAAAAAAAAAAAAAAABAgQDBgEBAQEBAAAAAAAAAAAAAAAAAAECBBABAAIBAgIIBgMBAAAAAAAAAAECERIDITFBUYGR0SITBGFxsTJCkqHBYhQRAQACAgMBAAAAAAAAAAAAAAABESECQVESMv/aAAwDAQACEQMRAD8A+BbxpjHT0rSuI1T2JLoKRFBEABBUAAAAAUAFRQFxqj4wixzFYHrp4xaOXTAHlbTxebVpZCQARAAEVAAAAUAABUUAAVvPCRnPAFtAkGRFyap+HdAINa56o/WPA9S3VX9a+AmWXp/z72mLacRaMxmYzjE2zjnyhi0zaczjsiI+jone2LbvrarRa1ZraunlmmjMTniJMzw5sTzxz5LTbve0VpGbTMRHa7J93szztea3/HHDbjRamK8eP3fw1t+82du9NNr1rtzSc1iM3ildMxPmGZ22r5cVdrcvia1m0TOmMdfUt9jdpSL2rMVtjE/OMx3w9fbe4ps0vW0TPq+S2PxrjnX/AF/XzN73NNzZ9OK6ZjR5oj7tNNHm+XQLe3qqx25xa2mvLHbET9V9S3VX9Y8BrLI1rnqjujwTVPw7oFAyABkFQWUVAAABABAUAEUQFEUAAABQABRDJYuEMmTAIvABBUQFQAAAUFAOBkFwJkyWKIFhKAgAAAAoCgAgAKCAgAAAAoAP/9k="},"9bcf":function(e,t,n){},a7f2:function(e,t,n){},a99d:function(e,t,n){"use strict";n("f28b")},c043:function(e,t,n){"use strict";n("d410")},c8ef:function(e,t,n){"use strict";n("9bcf")},d410:function(e,t,n){},ef31:function(e,t){e.exports="data:image/png;base64,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"},f28b:function(e,t,n){}});