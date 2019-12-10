@extends('layout')

<!--
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2019 by Daniel Brendel

    Version: 0.8
    Contact: dbrendel1988<at>yahoo<dot>com
    GitHub: https://github.com/danielbrendel

    Licence: Creative Commons Attribution-NoDerivatives 4.0 International
-->

@section('meta_description', 'View the latest community screenshots')
@section('meta_tags', 'screenshots, community')
@section('meta_title', 'Screenshots')

@section('body_content')
    <h1>Here you can find the last {{ env('SCREENSHOTS_LASTCOUNT') }} screenshots</h1>

    <div class="screenshot-container">
    @foreach ($screenshots as $screenshot)
        <div class="screenshot-img">
            <img src="img/uploads/{{ $screenshot->screenshot }}" alt="{{ $screenshot->screenshot }}"/>
        </div>
    @endforeach
    </div>
@endsection