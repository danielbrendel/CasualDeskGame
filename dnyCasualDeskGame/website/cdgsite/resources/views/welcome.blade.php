@extends('layout')

<!--
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2019 by Daniel Brendel

    Version: 0.8
    Contact: Daniel Brendel<at>gmail<dot>com
    GitHub: https://github.com/danielbrendel

    Licence: Creative Commons Attribution-NoDerivatives 4.0 International
-->

@section('meta_description', 'Home of Casual Desktop Game')
@section('meta_tags', 'home')
@section('meta_title', 'Home')

@section('body_content')
    <h1>Welcome to the offical homepage of Casual Desktop Game</h1>

    <p>
        Casual Desktop Game is a fun tool where you can spawn entities on your Desktop
        and handle them with various tools. It is completely customizable by creating
        own tools and entities via the scripting API. Therefore it utilizies AngelScript
        as scripting engine. New tools can be shared via the Workshop. It is available
        on Steam.
    </p>

    <strong>Casual Desktop Game is completely free!</strong>

    <p>
        Be sure to check out the <a href="https://steamcommunity.com/app/1001860">Steam community</a> page of the game!
    </p>

    <p align="center">
        <iframe width="560" height="500" src="https://www.youtube.com/embed/IoOLCVST48I" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
    </p>

    <h2>Wanna make an own tool? Check out the tutorial video!</h2>

    <p align="center">
        
        <iframe width="560" height="315" src="https://www.youtube.com/embed/ppzatxKGvLg" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
    </p>

    <p>
        Do you have questions? <a href="/contact">Contact</a> the developer!
    </p>
@endsection