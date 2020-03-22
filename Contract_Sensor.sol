/**
 *Submitted for verification at Etherscan.io on 2019-08-26
*/

/**
 *Submitted for verification at Etherscan.io on 2019-07-30
*/

/**

 (UTC) */

pragma solidity ^0.4.19;

// This contract receives values from distance values from the contract creator and stores them
// there are two types of stores
// one type get single value and store it separately in uint[].
// the other tyoe get value array and store them in uint[][]

contract NodeMCU_Endpoint {

    // Address of the contract creator. Only the creator is allowed to send values.
    address private creator;

    // Dynamic array of distance value
    uint[] public valueArray1;
    uint[][] public valueArray2;

    // Constructor defining the contract creator.
    function NodeMCU_Endpoint() public {
        creator = msg.sender;
    }

    // Modifier allowing only the contract creator to call a function.
    modifier onlyOwner() {
    require(msg.sender == creator);
    _;
    }

    // Create event log for each sent value.
    event OnSendData1(address sender, uint sentValue);
    event OnSendData2(address sender, uint[] sentValue);

    // type 1: get single distance value and judge if the object is still.
    function Send_Data1(uint amount) public onlyOwner {
        valueArray1.push(amount);
        OnSendData1(msg.sender, amount);
    }

    function Get_Last_i_Value_1(uint i) public view returns (uint) {
        if (valueArray1.length == 0) return;
        return valueArray1[valueArray1.length - i];
    }

    function Get_absolute_value(int a) internal view returns(uint){
        if (a>0) return uint(a);
        else return uint(-a);
    }

    function Get_Signal_1(int still_distace, uint tolerance) public view returns (bool){
        bool Signal_Still = true;// Whether the object is still
        int diff; //difference between value
        for(uint i=1;i<3;i++){
            diff = still_distace-int(valueArray1[valueArray1.length - i]);
            if (Get_absolute_value(diff)>tolerance ){
                Signal_Still = false;
                break;
            }
        }
        return Signal_Still;
    }


    //type 2: get valueArray of distance and just store it. no judge.
    function Send_Data2(uint[] amount) public onlyOwner {
        valueArray2.push(amount);
        OnSendData2(msg.sender, amount);
    }

        function Get_Last_i_Value_2(uint i) public view returns (uint[]) {
        if (valueArray2.length == 0) return;
        return valueArray2[valueArray2.length - i];
    }




}
