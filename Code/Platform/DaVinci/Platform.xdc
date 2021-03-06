/*!
 * File generated by platform wizard. DO NOT MODIFY
 *
 */

metaonly module Platform inherits xdc.platform.IPlatform {

    config ti.platforms.generic.Platform.Instance CPU =
        ti.platforms.generic.Platform.create("CPU", {
            clockRate:      594,                                       
            catalogName:    "ti.catalog.c6000",
            deviceName:     "TMS320CDM6437",
            externalMemoryMap: 
           [          
                ["DDR2", 
                     {
                        name: "DDR2",
                        base: 0x80000000,                    
                        len: 0x8000000,                    
                        space: "code/data",
                        access: "RWX",
                     }
                ],
                ["SRAM", 
                     {
                        name: "SRAM",
                        base: 0x42000000,                    
                        len: 0x0200000,                    
                        space: "code/data",
                        access: "RWX",
                     }
                ],
           ],
          l1DMode:"32k",
          l1PMode:"32k",
          l2Mode:"128k",

    });
    
instance :
    
    override config string codeMemory  = "SRAM";   
    override config string dataMemory  = "DDR2";                                
    override config string stackMemory = "L1DSRAM";
    
}
