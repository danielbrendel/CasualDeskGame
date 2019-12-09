<?php
    /*
        Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

        (C) 2018 - 2019 by Daniel Brendel

        Version: 0.8
        Contact: Daniel Brendel<at>gmail<dot>com
        GitHub: https://github.com/dny-coder

        Licence: Creative Commons Attribution-NoDerivatives 4.0 International
    */
?>

<!doctype html>
<html lang="en">
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <meta name="csrf-token" content="{{ csrf_token() }}">
        
        <meta name="author" content="Daniel Brendel">
        <meta name="description" content="@yield('meta_description')">
        <meta name="tags" content="cdg, casual desktop game, official, @yield('meta_tags')">

        <title>Casual Desktop Game - @yield('meta_title')</title>

        <link rel="stylesheet" type="text/css" href="css/bulma.css">
        <link rel="stylesheet" type="text/css" href="css/app.css">
        <script src="js/vue.js"></script>
        <script src="js/app.js"></script>
    </head>

    <body>
        <div id="app">
            <nav class="navbar is-black" role="navigation" aria-label="main navigation">
                <div class="navbar-brand">
                    <a class="navbar-item" href="{{ env('APP_URL') }}">
                        <img src="img/header.png" width="112" height="28">
                    </a>

                    <a role="button" class="navbar-burger burger" aria-label="menu" aria-expanded="false" data-target="navbarBasicExample" @click="navIsActive=!navIsActive">
                        <span aria-hidden="true"></span>
                        <span aria-hidden="true"></span>
                        <span aria-hidden="true"></span>
                    </a>
                </div>

                <div id="navbarBasicExample" class="navbar-menu" :class="{'is-active': navIsActive}">
                    <div class="navbar-start">
                        <a href="/" class="navbar-item">
                            Home
                        </a>

                        <a href="/download" class="navbar-item">
                            Download
                        </a>

                        <a href="/screenshots" class="navbar-item">
                            Screenshots
                        </a>

                        <a href="/contact" class="navbar-item">
                            Contact
                        </a>
                    </div>

                    <div class="navbar-end">
                    </div>
                </div>
            </nav>
            <div class="container">
                <div class="columns">
                    <div class="column">
                        <div class="content-bg">
                            <div class="content-cnt">
                                @yield('body_content')
                            </div>
                        </div>
                    </div>
                </div>
           </div>
        </div>

        <script language="javascript">
            new Vue({
                    el: '#app',
                    data: {
                        navIsActive: false
                    }
            });
        </script>
    </body>
</html>