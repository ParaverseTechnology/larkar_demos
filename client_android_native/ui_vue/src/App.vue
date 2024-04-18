<template>
  <router-view/>
</template>

<script>
export default {
  data() {
    return {
      isRouterAlive: true, //控制视图是否显示的变量
    }
  },
  mounted() {
    this.restoreHost();
    this.restoreSDKType();
    this.restoreQuickConfigLevel();
    this.updateStatus();

    console.log('global state', this.Host, this.isARReady, this.hasPermission, this.arcoreSupportStatus, this.selectedArSDKType);

    if (!this.Host) {
      this.$router.replace("/First");
    }
  },
  provide() {    //父组件中通过provide来提供变量，在子组件中通过inject来注入变量。
    return {
      reload: this.reload,
    }
  },
  methods:{
    reload() {
      this.isRouterAlive = false;            //先关闭，
      this.$nextTick(function () {
        console.log("qqqqqqqqqqq")
        this.isRouterAlive = true;         //再打开
      })
    }
  }

}
</script>

<style>
body {
  margin: 0;
  padding: 0;
  background-color: #f0f0f0;
}
</style>
