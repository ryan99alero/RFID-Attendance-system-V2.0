<?php
session_start();
if (!isset($_SESSION['Admin-name'])) {
    header("location: login.php");
}
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Manage Users</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" type="image/png" href="images/favicon.png">
    <link rel="stylesheet" type="text/css" href="css/manageusers.css">

    <script type="text/javascript" src="js/jquery-2.2.3.min.js"></script>
    <script src="https://code.jquery.com/jquery-3.3.1.js"
            integrity="sha1256-2Kok7MbOyxpgUVvAk/HJ2jigOSYS2auK4Pfzbm7uH60="
            crossorigin="anonymous">
    </script>
    <script type="text/javascript" src="js/bootbox.min.js"></script>
    <script type="text/javascript" src="js/bootstrap.js"></script>
    <script src="js/manage_users.js"></script>
    <script>
        $(window).on("load resize ", function () {
            const scrollWidth = $('.tbl-content').width() - $('.tbl-content table').width();
            $('.tbl-header').css({'padding-right': scrollWidth});
        }).resize();
    </script>
    <script>
        $(document).ready(function () {
            $.ajax({
                url: "manage_users_up.php"
            }).done(function (data) {
                $('#manage_users').html(data);
            });
            setInterval(function () {
                $.ajax({
                    url: "manage_users_up.php"
                }).done(function (data) {
                    $('#manage_users').html(data);
                });
            }, 5000);
        });
    </script>
</head>
<body>
<?php include 'header.php'; ?>
<main>
    <h1 class="slideInDown animated">Add a new User or update his information <br> or remove him</h1>
    <div class="form-style-5 slideInDown animated">
        <form enctype="multipart/form-data">
            <div class="alert_user"></div>
            <fieldset>
                <legend><span class="number">1</span> User Info</legend>
                <input type="hidden" name="user_id" id="user_id">
                <input type="text" name="first_name" id="first_name" placeholder="First Name...">
                <input type="text" name="last_name" id="last_name" placeholder="Last Name...">
                <input type="email" name="email" id="email" placeholder="User Email...">
                <input type="text" name="phone" id="phone" placeholder="Phone Number...">
                <input type="text" name="address" id="address" placeholder="Address...">
                <input type="text" name="city" id="city" placeholder="City...">
                <input type="text" name="state" id="state" placeholder="State...">
                <input type="text" name="zip" id="zip" placeholder="Zip...">
                <input type="text" name="country" id="country" placeholder="Country...">
                <input type="text" name="short_name" id="short_name" placeholder="Short Name...">
                <input type="text" name="payroll_id" id="payroll_id" placeholder="Payroll ID...">
                <input type="text" name="manager" id="manager" placeholder="Manager...">
                <input type="text" name="ssn" id="ssn" placeholder="Social Security Number...">
                <input type="date" name="start_date" id="start_date" placeholder="Start Date...">
                <input type="text" name="vacation_time" id="vacation_time" placeholder="Vacation Time...">
                <!-- Additional fields as per your requirement -->
            </fieldset>
            <fieldset>
                <legend><span class="number">2</span> Additional Info</legend>
                <label for="Device"><b>User Department:</b></label>
                <select class="dev_sel" name="dev_sel" id="dev_sel" style="color: #000;">
                    <option value="0">All Departments</option>
                    <?php
                    require 'connectDB.php';
                    $sql = "SELECT * FROM devices ORDER BY device_name ASC";
                    $result = mysqli_stmt_init($conn);
                    if (!mysqli_stmt_prepare($result, $sql)) {
                        echo '<p class="error">SQL Error</p>';
                    } else {
                        mysqli_stmt_execute($result);
                        $resultl = mysqli_stmt_get_result($result);
                        while ($row = mysqli_fetch_assoc($resultl)) {
                            ?>
                            <option value="<?php echo $row['device_uid']; ?>"><?php echo $row['device_dep']; ?></option>
                            <?php
                        }
                    }
                    ?>
                </select>
                <input type="radio" name="gender" class="gender" value="Female">Female
                <input type="radio" name="gender" class="gender" value="Male" checked="checked">Male
            </fieldset>
            <button type="button" name="user_add" class="user_add">Add User</button>
            <button type="button" name="user_upd" class="user_upd">Update User</button>
            <button type="button" name="user_rmo" class="user_rmo">Remove User</button>
        </form>
    </div>

    <!--User table-->
    <div class="section">
        <div class="slideInRight animated">
            <div id="manage_users"></div>
        </div>
    </div>
</main>
</body>
</html>
