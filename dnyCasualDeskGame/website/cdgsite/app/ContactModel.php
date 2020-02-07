<?php
/*
    Casual Desktop Game (dnycasualDeskGame) developed by Daniel Brendel

    (C) 2018 - 2020 by Daniel Brendel

    Version: 0.8
    Contact: dbrendel1988<at>yahoo<dot>com
    GitHub: https://github.com/danielbrendel

    License: see LICENSE.txt
*/

namespace App;

use Illuminate\Database\Eloquent\Model;

class ContactModel extends Model
{
    protected $fillable = ['name', 'address', 'email', 'message'];

    public static function addrMayContactAgain($addr)
    {
        //Check whether client may submit the contact form again

        $data = ContactModel::where('address', '=', $addr)->orderBy('created_at', 'desc')->first();
        if (time() - $data['created_at']->timestamp >  60 * 5)
            return true;

        return false;
    }

    public static function addContactData()
    {
        //Add contact data to database

        if (ContactModel::addrMayContactAgain(request()->ip())) { //Check if client may submit again
            //Add database entry

            $attributes = request()->validate([
                'name' => ['required', 'min:4', 'max:256'],
                'email' => ['required', 'min:6'. 'max:256', 'email'],
                'message' => ['required', 'min:5', 'max:2048']
            ]);
            
            $attributes['address'] = request()->ip();

            ContactModel::create($attributes);

            //Store flash message on success
            session()->flash('success', 'Thank you for contacting us.');
        } else {
            //Store flash message on error
            session()->flash('error', 'You must wait five minutes in order to contact again.');
        }
    }
}
