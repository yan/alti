//
//  SettingsViewController.swift
//  Alti
//
//  Created by user on 7/19/15.
//  Copyright (c) 2015 user. All rights reserved.
//

import UIKit

class SettingsViewController: UITableViewController {
    
//    @IBOutlet  weak var view: UITableView!
  //  var ds: UITableViewDataSource?
  
    override func viewDidLoad() {
        super.viewDidLoad()

//        ds = AlarmsViewDataSource()
//        (self.view as! UITableView).dataSource = self;


        // Uncomment the following line to preserve selection between presentations
        // self.clearsSelectionOnViewWillAppear = false

        // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
//        self.navigationItem.rightBarButtonItem = self.editButtonItem()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    // MARK: - Table view data source
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell
    {
        func getAndSetCell(title: String) -> UITableViewCell {
            let tvc = tableView.dequeueReusableCellWithIdentifier("alarm") as! UITableViewCell
            (tvc.contentView.viewWithTag(1) as! UILabel).text = title
            (tvc.contentView.viewWithTag(2) as! UISwitch).enabled = false;
            return tvc
        }
        switch indexPath.section {
        case 0:
            switch indexPath.item {
            case 0: return getAndSetCell("High Precision")
            case 1: return getAndSetCell("GPS")
            default: assert(false, "")
            }
        case 1:
            switch indexPath.item {
            case 0: return getAndSetCell("1,000\"")
            case 1: return getAndSetCell("10,000\"")
            default: assert(false, "")
            }
        case 2:
            return getAndSetCell("One")
        case 3:
            return getAndSetCell("One")
        default:
            assert(false, "")
        }


        println("Getting row \(indexPath)")
    }
    
    override func tableView(tableView: UITableView, numberOfRowsInSection section: Int) -> Int
    {
        println("Asked how many rows in section \(section)")
        
        
        switch section {
        case 0 ..< 2:
            return 2;
        case 2:
            return 3;
        default:
            return 0;
        }
    }
    override func numberOfSectionsInTableView(tableView: UITableView) -> Int {
        return 4;
    }
    
    
    override func tableView(tableView: UITableView, titleForHeaderInSection section: Int) -> String? {
        switch section {
        case 0: return "Logging"
        case 1: return "Airplane Alarms"
        case 2: return "Freefall Alarms"
        case 3: return "Canopy Alarms"
        default:
            assert(false, "Bad section number")
        }
    }
    /*
    override func tableView(tableView: UITableView, cellForRowAtIndexPath indexPath: NSIndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCellWithIdentifier("reuseIdentifier", forIndexPath: indexPath) as! UITableViewCell

        // Configure the cell...

        return cell
    }
    */
    // MARK: Slider delegate
    @IBAction func toggleSwitch(sender: UISwitch) {
        println("Changed");
    }
    /*
    // Override to support conditional editing of the table view.
    override func tableView(tableView: UITableView, canEditRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the specified item to be editable.
        return true
    }
    */

    /*
    // Override to support editing the table view.
    override func tableView(tableView: UITableView, commitEditingStyle editingStyle: UITableViewCellEditingStyle, forRowAtIndexPath indexPath: NSIndexPath) {
        if editingStyle == .Delete {
            // Delete the row from the data source
            tableView.deleteRowsAtIndexPaths([indexPath], withRowAnimation: .Fade)
        } else if editingStyle == .Insert {
            // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
        }    
    }
    */

    /*
    // Override to support rearranging the table view.
    override func tableView(tableView: UITableView, moveRowAtIndexPath fromIndexPath: NSIndexPath, toIndexPath: NSIndexPath) {

    }
    */

    /*
    // Override to support conditional rearranging of the table view.
    override func tableView(tableView: UITableView, canMoveRowAtIndexPath indexPath: NSIndexPath) -> Bool {
        // Return NO if you do not want the item to be re-orderable.
        return true
    }
    */

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using [segue destinationViewController].
        // Pass the selected object to the new view controller.
    }
    */

}
