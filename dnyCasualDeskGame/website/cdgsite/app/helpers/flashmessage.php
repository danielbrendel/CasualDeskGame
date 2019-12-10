<?php

/*
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2019 by Daniel Brendel

    Version: 0.8
    Contact: dbrendel1988<at>yahoo<dot>com
    GitHub: https://github.com/danielbrendel

    Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

function flash($message)
{
    session()->flash('message', $message);
}