from flask import Flask, render_template, request, redirect, url_for, session, flash
import subprocess, os

app = Flask(__name__)
app.secret_key = "rapid_secret_key"

# ---------- Backend Setup ----------
BACKEND_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "backend"))
EXE_PATH = os.path.join(BACKEND_DIR, "rapid.exe")

def run_backend(args):
    """Run compiled C backend and return its output safely (Windows-friendly)."""
    if not os.path.exists(EXE_PATH):
        return f"Error: Backend not found at {EXE_PATH}"

    try:
        result = subprocess.run(
            [EXE_PATH] + args,
            cwd=BACKEND_DIR,
            capture_output=True,
            text=True,
            encoding="utf-8",
            errors="ignore",
            timeout=20
        )

        # Safe handling of both stdout and stderr
        stdout = result.stdout or ""
        stderr = result.stderr or ""

        if result.returncode != 0:
            return f"Backend error ({result.returncode}):\n{stderr.strip() or '(no stderr output)'}"
        return stdout.strip() or "(no output from backend)"

    except subprocess.TimeoutExpired:
        return "Backend timed out."
    except Exception as e:
        return f"Error running backend: {e}"


# ---------- Home ----------
@app.route("/")
def index():
    return render_template("index.html")

# ---------- Login ----------
USERS_FILE = os.path.join(BACKEND_DIR, "data", "users.txt")

def read_users():
    if not os.path.exists(USERS_FILE):
        return []
    with open(USERS_FILE, "r") as f:
        return [line.strip().split() for line in f if line.strip()]

def write_user(username, password):
    os.makedirs(os.path.dirname(USERS_FILE), exist_ok=True)
    with open(USERS_FILE, "a") as f:
        f.write(f"{username} {password}\n")



@app.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        role = request.form.get("role")
        if role == "admin":
            return redirect(url_for("login_admin"))
        else:
            return redirect(url_for("login_user"))
    return render_template("login.html")

@app.route("/login_user", methods=["GET", "POST"])
def login_user():
    if request.method == "POST":
        username = request.form["username"]
        password = request.form["password"]
        users = read_users()
        for u, p in users:
            if u == username and p == password:
                session["username"] = username
                flash("Login successful!", "success")
                return redirect(url_for("user_dashboard"))
        flash("Invalid credentials. Please try again.", "danger")
    return render_template("login_user.html")

@app.route("/signup", methods=["GET", "POST"])
def signup():
    if request.method == "POST":
        username = request.form["username"]
        password = request.form["password"]
        users = read_users()
        for u, _ in users:
            if u == username:
                flash("Username already exists.", "warning")
                return redirect(url_for("signup"))
        write_user(username, password)
        flash("Signup successful! Please log in.", "success")
        return redirect(url_for("login_user"))
    return render_template("signup.html")

@app.route("/logout")
def logout():
    session.pop("username", None)
    flash("You have been logged out.", "info")
    return redirect(url_for("index"))

# ---------- User ----------
@app.route("/user_dashboard")
def user_dashboard():
    if "username" not in session:
        return redirect(url_for("login_user"))
    return render_template("user_dashboard.html", username=session["username"])

@app.route("/report", methods=["GET", "POST"])
def report():
    if "username" not in session:
        return redirect(url_for("login_user"))
    if request.method == "POST":
        user_id = session["username"]
        location = request.form["location"]
        severity = request.form["severity"]
        contact = request.form["contact"]
        output = run_backend(["report", user_id, location, severity, contact])
        return render_template("status.html", data=output.splitlines())
    return render_template("report.html")


# ---------- Admin ----------
@app.route("/admin_dashboard")
def admin_dashboard():
    if "admin" not in session:
        flash("Please log in as admin first.", "warning")
        return redirect(url_for("login_admin"))
    data_dir = os.path.join(BACKEND_DIR, "data")
    stats = {"total_ambulances": 0, "available": 0, "busy": 0, "hospitals": 0, "pending": 0}
    ambf = os.path.join(data_dir, "ambulances.txt")
    hospf = os.path.join(data_dir, "hospitals.txt")
    reqf = os.path.join(data_dir, "requests.txt")

    if os.path.exists(ambf):
        with open(ambf) as f:
            for line in f:
                if not line.strip(): continue
                stats["total_ambulances"] += 1
                if "available" in line: stats["available"] += 1
                if "busy" in line: stats["busy"] += 1
    if os.path.exists(hospf):
        with open(hospf) as f:
            stats["hospitals"] = len([l for l in f if l.strip()])
    if os.path.exists(reqf):
        with open(reqf) as f:
            stats["pending"] = len([l for l in f if l.strip()])

    return render_template("admin_dashboard.html", stats=stats)

@app.route("/add_ambulance", methods=["GET", "POST"])
def add_ambulance():
    if request.method == "POST":
        aid = request.form["id"]
        loc = request.form["location"]
        out = run_backend(["add_ambulance", aid, loc])
        flash(out)
        return redirect(url_for("admin_dashboard"))
    return render_template("add_ambulance.html")

@app.route("/add_hospital", methods=["GET", "POST"])
def add_hospital():
    if request.method == "POST":
        hid = request.form["id"]
        loc = request.form["location"]
        beds = request.form["beds"]
        out = run_backend(["add_hospital", hid, loc, beds])
        flash(out)
        return redirect(url_for("admin_dashboard"))
    return render_template("add_hospital.html")

@app.route("/update_status", methods=["GET", "POST"])
def update_status():
    if request.method == "POST":
        aid = request.form["id"]
        status = request.form["status"]
        out = run_backend(["update_status", aid, status])
        flash(out)
        return redirect(url_for("admin_dashboard"))
    return render_template("update_status.html")

@app.route("/check_status")
def check_status():
    if "username" not in session:
        return redirect(url_for("login_user"))
    user_id = session["username"]
    out = run_backend(["check_status", user_id])
    return render_template("status.html", data=out.splitlines())


@app.route("/view_ambulances")
def view_ambulances():
    out = run_backend(["view_ambulances"])
    return render_template("view_ambulances.html", data=out.splitlines())

@app.route("/view_hospitals")
def view_hospitals():
    out = run_backend(["view_hospitals"])
    return render_template("view_hospitals.html", data=out.splitlines())

@app.route("/view_requests")
def view_requests():
    out = run_backend(["view_requests"])
    return render_template("view_requests.html", data=out.splitlines())

@app.route("/approve_request", methods=["POST"])
def approve_request():
    out = run_backend(["approve_request"])
    return render_template("approve_status.html", data=out.splitlines())

@app.route("/find_ambulance", methods=["GET", "POST"])
def find_ambulance():
    if request.method == "POST":
        amb_id = request.form["id"]
        out = run_backend(["find_ambulance", amb_id])
        return render_template("find_ambulance.html", data=out.splitlines())
    return render_template("find_ambulance.html")


@app.route("/view_completed")
def view_completed():
    out = run_backend(["view_completed"])
    return render_template("view_completed.html", data=out.splitlines())
@app.route("/login_admin", methods=["GET", "POST"])
def login_admin():
    if request.method == "POST":
        username = request.form["username"]
        password = request.form["password"]

        if username == "admin" and password == "admin123":
            session["admin"] = True
            flash("Admin login successful!", "success")
            return redirect(url_for("admin_dashboard"))
        else:
            flash("Invalid admin credentials!", "danger")

    return render_template("login_admin.html")


# ---------- Run ----------
if __name__ == "__main__":
    app.run(debug=True)
