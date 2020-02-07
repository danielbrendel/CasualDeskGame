<?php

/*
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2020 by Daniel Brendel

    Version: 0.8
    Contact: dbrendel1988<at>yahoo<dot>com
    GitHub: https://github.com/danielbrendel

    License: see LICENSE.txt
*/

function flash($message)
{
    session()->flash('message', $message);
}