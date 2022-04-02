import { createRouter, createWebHashHistory } from 'vue-router'
import Index from '../views/Index.vue'
import WebClient from '../views/WebClient.vue';
import Setup from '../views/Setup';
import First from '../views/First';

const routes = [
  {
    path: '/',
    name: 'Index',
    component: Index
  },
  {
    path: '/WebClient',
    name: 'WebClient',
    component: WebClient,
  },
  {
    path: '/Setup',
    name: 'Setup',
    component: Setup,
  },
  {
    path: '/First',
    name: 'First',
    component: First,
  },
  {
    path: '/about',
    name: 'About',
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component: () => import(/* webpackChunkName: "about" */ '../views/About.vue')
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

export default router
