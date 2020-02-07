<?php

/*
	Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2020 by Daniel Brendel

	Version: 0.8
	Contact: dbrendel1988<at>yahoo<dot>com
	GitHub: https://github.com/danielbrendel

	License: see LICENSE.txt
*/

namespace App\Http\Controllers;

use Illuminate\Http\Request;
use App\ContactModel;
use App\ScreenshotModel;

class NavController extends Controller
{
    public function index()
    {
        //Return index view

        return view('welcome');
    }

    public function download()
    {
        //Return download view

        return view('download');
    }

    public function screenshots()
    {
        //Return screenshots view

        return view('screenshots', ['screenshots' => ScreenshotModel::queryLastScreenshots()]);
    }

    public function contact()
    {
        //Return contact view

        return view('contact');
    }

    public function doContact()
    {
        //Perform contact data storage

        ContactModel::addContactData();

        return back();
    }

    public function doUploadScreenshot()
    {
        //Perform screenshot upload
        
        ScreenshotModel::storeScreenshot();

        return back();
    }
}
