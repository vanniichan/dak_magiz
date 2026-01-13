$computer = "."
$namespace = "root\subscription"
$filterName = "CalculatorFilter"      
$consumerName = "NotepadConsumer"

# Delete old Binding
Get-WmiObject -Namespace $namespace -Class __FilterToConsumerBinding | 
    Where-Object { $_.Filter -match $filterName -or $_.Consumer -match $consumerName } | 
    Remove-WmiObject
Write-Host "[-] Done"

# Delete old Consumer
Get-WmiObject -Namespace $namespace -Class CommandLineEventConsumer | 
    Where-Object { $_.Name -eq $consumerName } | 
    Remove-WmiObject
Write-Host "[-] Done"

# Delete old Filter
Get-WmiObject -Namespace $namespace -Class __EventFilter | 
    Where-Object { $_.Name -eq $filterName } | 
    Remove-WmiObject
Write-Host "[-] Done"

# Wait 2s to update
Start-Sleep -Seconds 2 

# Listening CalculatorApp.exe 
$filterQuery = "SELECT * FROM __InstanceCreationEvent WITHIN 1 WHERE TargetInstance ISA 'Win32_Process' AND TargetInstance.Name='CalculatorApp.exe'"

$filterArgs = @{
    Name = $filterName
    EventNamespace = "root\cimv2"
    QueryLanguage = "WQL"
    Query = $filterQuery
}
$filter = Set-WmiInstance -Class __EventFilter -ComputerName $computer -Namespace $namespace -Arguments $filterArgs
Write-Host "[+] Done" -ForegroundColor Green

# Create Consumer
$consumerArgs = @{
    Name = $consumerName
    ExecutablePath = "notepad.exe"
    CommandLineTemplate = "notepad.exe"
}
$consumer = Set-WmiInstance -Class CommandLineEventConsumer -ComputerName $computer -Namespace $namespace -Arguments $consumerArgs
Write-Host "[+] Done" -ForegroundColor Green

# Create Binding
if ($filter -and $consumer) {
    $bindingArgs = @{
        Filter = $filter
        Consumer = $consumer
    }
    Set-WmiInstance -Class __FilterToConsumerBinding -ComputerName $computer -Namespace $namespace -Arguments $bindingArgs
    Write-Host "[+] Done" -ForegroundColor Cyan
    Write-Host ">>> Open Calculator" -ForegroundColor Yellow
} else {
    Write-Host "[!] Error" -ForegroundColor Red
}
