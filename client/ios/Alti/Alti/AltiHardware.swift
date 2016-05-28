//
//  AltiHardware.swift
//  Alti
//
//  Created by user on 6/25/15.
//  Copyright (c) 2015 user. All rights reserved.
//

import Foundation
import CoreBluetooth
import BlueCapKit

extension UInt32 : Deserializable {
    
    public static var size : Int {
        return sizeof(UInt32)
    }
    
    public init?(stringValue:String) {
        if let value = Int(stringValue) {
            self = UInt32(value)
        } else {
            return nil
        }
    }
    
    public init?(doubleValue:Double) {
        self = UInt32(doubleValue)
    }
    
    public static func deserialize(data:NSData) -> UInt32? {
        if data.length >= sizeof(UInt32) {
            var value : UInt32 = 0
            data.getBytes(&value , length:sizeof(UInt32))
            return value
        } else {
            return nil
        }
    }
    
    public static func deserialize(data:NSData, start:Int) -> UInt32? {
        if data.length >= (sizeof(UInt32) + start)  {
            var value : UInt32 = 0
            data.getBytes(&value, range:NSMakeRange(start, sizeof(UInt32)))
            return value
        } else {
            return nil
        }
    }
    
    public static func deserialize(data:NSData) -> [UInt32] {
        let size = sizeof(UInt32)
        let count = data.length / size
        return [Int](0..<count).reduce([]) {(result, idx) in
            if let value = self.deserialize(data, start:idx*size) {
                return result + [value]
            } else {
                return result
            }
        }
    }
    
}


public struct AltiHardware {
    public struct StreamingPressureService : ServiceConfigurable {
        public static let uuid = "701B0001-9DDC-4053-BC77-410A972965F7"
        public static let name = "Streaming Pressure Service"
        public static let tag  = "Barometric Pressure Sensor"
 
        
        /*
        typealias RawType
        static var uuid         : String {get}
        var rawValue            : RawType {get}
        init?(rawValue:RawType)
*/
        
        public struct Baro : RawDeserializable  {
            public var rawValue : UInt32
            
            public static var uuid = "701B0002-9DDC-4053-BC77-410A972965F7"
            
            public init?(rawValue: UInt32) {
                self.rawValue = rawValue
            }
            
            func toAltitude() -> Double {
                let p = Double(self.rawValue) / 100
                let presAlt = (1 - pow(p/1013.25, 0.190284)) * 145366.45
                return presAlt
                
            }
        }
    }
}