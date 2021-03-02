$port= new-Object System.IO.Ports.SerialPort COM4,250000,None,8,one
$port.open()

$response = ""

# [Parameter(Mandatory)]
#         [ConsoleKey]
#         $Key,

do
{
	# $line = $port.ReadLine();
    # Write-Host $line # Do stuff here

    # if ($port.IsOpen)
    # {
    #     $data = $port.ReadExisting()
    #     if($data.Length -gt 0)
    #     {
    #         $data = $data -replace [System.Environment]::NewLine,""
    #         Write-Host  $data
    #     }
        
    # }
    
    Write-Host $port.ReadLine();

    # wait for a key to be available:
    if ([Console]::KeyAvailable)
    {
        Write-Host "=========================================Available" # Do stuff here
        # read the key, and consume it so it won't
        # be echoed to the console:
        $keyPress = [Console]::ReadKey($true)
        Write-Host $keyPress.Character

        # $key -eq $pressedKey.Key
        if(($keyPress.KeyChar  -eq 'q') -or (keyPress.KeyChar -eq 'Q'))
        {
            # exit loop
            $port.Close()
            Write-Host "==========>>>>>>>>>>>>>>>>>Closing Serial port" # Do stuff here
            break
        }
    }
}
 while($port.IsOpen)