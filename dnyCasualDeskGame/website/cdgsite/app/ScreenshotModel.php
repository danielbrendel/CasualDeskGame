<?php
/*
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2019 by Daniel Brendel

    Version: 0.8
    Contact: Daniel Brendel<at>gmail<dot>com
    GitHub: https://github.com/danielbrendel

    Licence: Creative Commons Attribution-NoDerivatives 4.0 International
*/

namespace App;

use Illuminate\Database\Eloquent\Model;
use Illuminate\Support\Facades\Storage;

class ScreenshotModel extends Model
{
    protected $fillable = ['address', 'screenshot', 'steamname'];

    public static function mayUploadScreenshot($addr)
    {
        //Check whether the client may upload a screenshot again

        $data = ScreenshotModel::where('address', '=', $addr)->orderBy('created_at', 'desc')->first();
        
        if ($data == null)
            return true;

        if (time() - $data['created_at']->timestamp >  60)
            return true;
        
        return false;
    }

    private static function convertToPNG($file)
    {
        //Convert BMP image to PNG

        //Generate new name
        $newname = hash('md5', $file) . '.png';

        //Copy the file to the public directory
        $content = Storage::get($file);
        $filehandle = fopen('img/uploads/' . $file, 'wb');
        fwrite($filehandle, $content);
        fclose($filehandle);

        //Convert to PNG
        $image = imagecreatefrombmp('img/uploads/' . $file);
        imagepng($image, 'img/uploads/' . $newname);
        imagedestroy($image);

        //Delete temp files
        unlink('img/uploads/' . $file);
        Storage::delete($file);

        return $newname;
    }

    public static function storeScreenshot()
    {
        //Store screenshot

        if (ScreenshotModel::mayUploadScreenshot(request()->ip())) { //Check if client is allowed to upload again
            //Store file to disk
            $file = request()->file('screenshot')->store('');
            
            //Convert to PNG
            $newname = ScreenshotModel::convertToPNG($file);

            //Create database entry

            $attributes = request()->validate([
                'steamname' => ['required', 'max:256']
            ]);

            $attributes['address'] = request()->ip();
            $attributes['screenshot'] = $newname;

            ScreenshotModel::create($attributes);
        }
    }

    public static function queryLastScreenshots()
    {
        //Query last N screenshots

        $screenshots = ScreenshotModel::orderBy('id', 'desc')->take(env('SCREENSHOTS_LASTCOUNT'))->get();

        return $screenshots;
    }
}
