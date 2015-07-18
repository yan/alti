//
//  SettingsController.swift
//  Alti
//
//  Created by user on 6/25/15.
//  Copyright (c) 2015 user. All rights reserved.
//

import UIKit
import CoreBluetooth
import BlueCapKit

public enum CentralExampleError : Int {
    case DataCharactertisticNotFound        = 1
    case EnabledCharactertisticNotFound     = 2
    case ServiceNotFound                    = 3
    case CharacteristicNotFound             = 4
    case PeripheralNotConnected             = 5
}

public struct CenteralError {
    public static let domain = "Central Example"
    public static let dataCharacteristicNotFound = NSError(domain:domain, code:CentralExampleError.DataCharactertisticNotFound.rawValue, userInfo:[NSLocalizedDescriptionKey:"Accelerometer Data Chacateristic Not Found"])
    public static let enabledCharacteristicNotFound = NSError(domain:domain, code:CentralExampleError.EnabledCharactertisticNotFound.rawValue, userInfo:[NSLocalizedDescriptionKey:"Accelerometer Enabled Chacateristic Not Found"])
    public static let serviceNotFound = NSError(domain:domain, code:CentralExampleError.ServiceNotFound.rawValue, userInfo:[NSLocalizedDescriptionKey:"Accelerometer Service Not Found"])
    public static let characteristicNotFound = NSError(domain:domain, code:CentralExampleError.CharacteristicNotFound.rawValue, userInfo:[NSLocalizedDescriptionKey:"Accelerometer Characteristic Not Found"])
    public static let peripheralNotConnected = NSError(domain:domain, code:CentralExampleError.PeripheralNotConnected.rawValue, userInfo:[NSLocalizedDescriptionKey:"Peripheral not connected"])
}



class SettingsController: UIViewController {

    @IBOutlet weak var labelAltitude: UILabel!
    @IBOutlet weak var labelPressure: UILabel!
    @IBOutlet weak var labelStatus: UILabel!
    
    var peripheral            : Peripheral?
    var baroCharacteristic    : Characteristic?
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        let serviceUUID = CBUUID(string: AltiHardware.StreamingPressureService.uuid)
        let baroUUID = CBUUID(string: AltiHardware.StreamingPressureService.Baro.uuid)

        // on power, start scanning. when peripheral is discovered connect and stop scanning
        let manager = CentralManager.sharedInstance

        let peripheralConnectFuture = manager.powerOn().flatmap {_ -> FutureStream<Peripheral> in
            println("Started Scanning")
            return manager.startScanningForServiceUUIDs([serviceUUID], capacity:10)
        }.flatmap {peripheral -> FutureStream<(Peripheral, ConnectionEvent)> in
            self.peripheral = peripheral

            manager.stopScanning()
            
            println("Got peripheral")
            
            return peripheral.connect(capacity:10, timeoutRetries:5, disconnectRetries:5)
        }
        
        
        
        
        
        
        peripheralConnectFuture.onSuccess {(peripheral, connectionEvent) in
            self.updateUI()
            
            switch (connectionEvent) {
//            case .Connect:
//                peripheral.discoverPeripheralServices([serviceUUID])
            case .Timeout:
                peripheral.reconnect()
            case .Disconnect:
                peripheral.reconnect()
//            case .ForceDisconnect:
//            case .Failed:
            case .GiveUp:
                peripheral.terminate()
            default:
                return
            }
        }
        
       let characteristicsDiscoveredFuture = peripheralConnectFuture.flatmap {(peripheral, connectionEvent) -> Future<Peripheral> in
            if peripheral.state == .Connected {
                return peripheral.discoverPeripheralServices([serviceUUID])
            } else {
                let promise = Promise<Peripheral>()
                promise.failure(CenteralError.peripheralNotConnected)
                return promise.future
            }
        }
        

        let subscribedCharacteristicFuture = characteristicsDiscoveredFuture.flatmap { peripheral -> Future<Characteristic> in
            if let service = peripheral.service(serviceUUID), characteristic = service.characteristic(baroUUID) {
                return characteristic.startNotifying()
            } else {
                let promise = Promise<Characteristic>()
                promise.failure(CenteralError.characteristicNotFound)
                return promise.future
            }
        }
        
        subscribedCharacteristicFuture.flatmap { characteristic -> FutureStream<Characteristic> in
            return characteristic.recieveNotificationUpdates(capacity: 10)
        }.onSuccess { c in
            self.updateData(c)
        }
        

    }

    func updateData(c : Characteristic) {
        
        if let v : AltiHardware.StreamingPressureService.Baro = c.value() {
            println(v)
            self.labelPressure.text = "[\(v.rawValue)]"
            self.labelAltitude.text = NSString(format: "%.2f", v.toAltitude()) as String
        }
    }
    
    func updateUI() -> Void {
        if let peripheral = self.peripheral {
            switch peripheral.state {
            case .Connected:
                self.labelStatus.text = "Connected!"
            case .Connecting:
                self.labelStatus.text = "Connecting"
            case .Disconnected:
                self.labelStatus.text = "Disconnected."
            }
        }
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
