@extends('layout')

<!--
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2020 by Daniel Brendel

    Version: 0.8
    Contact: dbrendel1988<at>yahoo<dot>com
    GitHub: https://github.com/danielbrendel

    License: see LICENSE.txt
-->

@section('meta_description', 'Contact the game author')
@section('meta_tags', 'contact')
@section('meta_title', 'Contact')

@section('body_content')
    <h1>Contact the developer</h1>

    <p>
        <contact-form></contact-form>

        @if (Session::has('success')) 
        <div class="fm-bg fm-issuccess">
            <div class="fm-msg">{{ Session::get('success') }}</div>
        </div>
        @endif

        @if (Session::has('error')) 
        <div class="fm-bg fm-iserror">
            <div class="fm-msg">{{ Session::get('error') }}</div>
        </div>
        @endif
    </p>
@endsection