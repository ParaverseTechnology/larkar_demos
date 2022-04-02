<template>
  <router-view/>
</template>

<script>
export default {
  mounted() {
    this.restoreHost();
    this.restoreSDKType();
    this.updateStatus();
    
    console.log('global state', this.Host, this.isARReady, this.hasPermission, this.arcoreSupportStatus, this.selectedArSDKType);

    if (!this.Host) {
      this.$router.replace("/First");
    }
  },
}
</script>

<style>
body {
  margin: 0;
  padding: 0;
  background-color: #f0f0f0;
}
</style>
