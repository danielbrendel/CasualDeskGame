<?php

/*
	Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

	(C) 2018 - 2019 by Daniel Brendel

	Version: 0.8
	Contact: Daniel Brendel<at>gmail<dot>com
	GitHub: https://github.com/dny-coder

	Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider within a group which
| contains the "web" middleware group. Now create something great!
|
*/

Route::get('/', 'NavController@index');
Route::get('/download', 'NavController@download');
Route::get('/screenshots', 'NavController@screenshots');
Route::get('/contact', 'NavController@contact');
Route::post('/contact', 'NavController@doContact');
Route::post('/uploadScreenshot', 'NavController@doUploadScreenshot');
