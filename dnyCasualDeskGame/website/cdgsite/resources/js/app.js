/**
 * First we will load all of this project's JavaScript dependencies which
 * includes Vue and other libraries. It is a great starting point when
 * building robust, powerful web applications using Vue and Laravel.
 */

require('./bootstrap');

window.Vue = require('vue');

/**
 * The following block of code may be used to automatically register your
 * Vue components. It will recursively scan this directory for the Vue
 * components and automatically register them with their "basename".
 *
 * Eg. ./components/ExampleComponent.vue -> <example-component></example-component>
 */

// const files = require.context('./', true, /\.vue$/i);
// files.keys().map(key => Vue.component(key.split('/').pop().split('.')[0], files(key).default));

Vue.component('example-component', require('./components/ExampleComponent.vue').default);

/*
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2019 by Daniel Brendel

    Version: 0.8
    Contact: Daniel Brendel<at>gmail<dot>com
    GitHub: https://github.com/danielbrendel

    Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/
Vue.component('contact-form', { //A contact form component
    template: `
        <div>
        <form method="POST" action="/contact">
            <input type="hidden" name="_token" :value="csrf">

            <div class="field">
            <label class="label">Name</label>
            <div class="control">
                <input name="name" v-model="inpName" class="input" type="text" placeholder="Your name" @keydown="formElementsValid()">
            </div>
            <p class="help is-danger" v-show="isNotValidName">Your name must exceed two characters and be less than 256 characters</p>
            </div>

            <div class="field">
            <label class="label">Email</label>
            <div class="control has-icons-left has-icons-right">
                <input name="email" v-model="inpEmail" class="input" type="email" placeholder="name@example.com" @keydown="formElementsValid()">
                <span class="icon is-small is-left">
                <i class="fas fa-envelope"></i>
                </span>
                <span class="icon is-small is-right">
                <i class="fas fa-exclamation-triangle"></i>
                </span>
            </div>
            <p class="help is-danger" v-show="isNotValidEmail">This email is invalid</p>
            </div>

            <div class="field">
            <label class="label">Message</label>
            <div class="control">
                <textarea name="message" v-model="inpMessage" class="textarea" placeholder="Your detailed message" @keydown="formElementsValid()"></textarea>
            </div>
            <p class="help is-danger" v-show="isNotValidMessage">The message must be more than five and less than 2048 characters</p>
            </div>
            
            <div class="field is-grouped">
            <div class="control">
                <button id="formSubmitButton" class="button is-link" type="submit" disabled>Submit</button>
            </div>
            
            <div class="control">
                <button class="button is-text" type="reset" @click="clearForm()">Cancel</button>
            </div>
            </div>
        </form>
        </div>
    `,

    data: function() {
        return {
            inpName: '',
            inpEmail: '',
            inpMessage: '',
            csrf: document.querySelector('meta[name="csrf-token"]').getAttribute('content')
        }
    },

    computed: {
        isNotValidName: function() {
            return (this.inpName.length > 0) && ((this.inpName.length < 2) || (this.inpName.length > 256));
        },

        isNotValidEmail: function() {
            return ((this.inpEmail.indexOf('@') == -1) || (this.inpEmail.indexOf('.') == -1) || (this.inpEmail.length <= 5)) && (this.inpEmail.length > 0);
        },

        isNotValidMessage: function() {
            return (this.inpMessage.length > 0) && ((this.inpMessage.length < 5) || (this.inpMessage.length > 2048));
        }
    },

    methods: {
        formElementsValid: function() {
            //Check form elements for being valid
            let oSubmitBtn = document.getElementById("formSubmitButton");
            if (((this.inpName.length > 2) && (this.inpName.length < 256)) && ((this.inpEmail.length >= 5) && (this.inpEmail.indexOf('@') > 0) && (this.inpEmail.indexOf('.') > 0)) && ((this.inpMessage.length > 5) && (this.inpMessage.length < 2048))) {
                oSubmitBtn.disabled = false;
            } else {
                oSubmitBtn.disabled = true;
            }
        },

        clearForm: function() {
            //Clear form elements model data
            this.inpName = this.inpEmail = this.inpMessage = '';
        }
    }
});

/**
 * Next, we will create a fresh Vue application instance and attach it to
 * the page. Then, you may begin adding components to this application
 * or customize the JavaScript scaffolding to fit your unique needs.
 */

const app = new Vue({
    el: '#app',
});
