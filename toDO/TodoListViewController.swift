//
//  ViewController.swift
//  toDO
//
//  Created by skander lakhal on 2/19/19.
//  Copyright © 2019 REDRebelRiots. All rights reserved.
//

import UIKit

class TodoListViewController: UITableViewController {
    
    var itemArray = ["tothings", "studyIOS", "Procastinationexpertise"]
    
    let defaults = UserDefaults.standard
    
    

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        
        self.tableView.separatorStyle = UITableViewCell.SeparatorStyle.none
        if let items = defaults.array(forKey: "TodoListArray") as? [String] {
            itemArray = items
        }
    }
    
    
    override func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        
        return itemArray.count
    }
    
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {

        let cell = tableView.dequeueReusableCell(withIdentifier: "ToDoItemCell", for: indexPath)
        
       cell.textLabel?.text = itemArray[indexPath.row]
        
        return cell
        
        
    }
    
    // MARK: create tableview delegate method gets started whenever table view cell pressed
    
    
    override func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
    
       // print(itemArray[indexPath.row])
        
      
        
        if tableView.cellForRow(at: indexPath)?.accessoryType == .checkmark {
            tableView.cellForRow(at: indexPath)?.accessoryType = .none
        }else{
            tableView.cellForRow(at: indexPath)?.accessoryType = .checkmark
        }
        
        tableView.deselectRow(at: indexPath, animated: true)
    }
    
   //MARK add new items
    
    
    @IBAction func addButtonPressed(_ sender: UIBarButtonItem) {
        
        var textField = UITextField()
        
        let alert = UIAlertController(title:"add new Item", message: "", preferredStyle: .alert)
        
        let action = UIAlertAction(title: "Add Item", style: .default) { (action) in
            // what will happen once user clicks press add button on UIalert
            print ("Succcess")
           
            
            self.itemArray.append(textField.text!)
            
            self.defaults.set(self.itemArray, forKey: "TodoListArray")
            self.tableView.reloadData()
        
        }
        alert.addTextField { (alertTextFiled) in
            alertTextFiled.placeholder = "Create new item"
            textField = alertTextFiled
            

        }
        alert.addAction(action)
        present(alert, animated: true, completion: nil)
       
    }
    

}

